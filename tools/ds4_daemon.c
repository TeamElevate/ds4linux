#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>
#include <ds4_data.h>

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  keep_running = 0;

  sigaction(sig, &sa, NULL);
}

void set_handlers() {
  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
}

int setup_unix_socket() {
  unsigned int s;
  int len;
  int rc;
  struct sockaddr_un local;

  s = socket(AF_UNIX, SOCK_STREAM, 0);
  if (s < 0) return s;

  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, "/opt/controller.ipc");
  unlink("/opt/controller.ipc");
  len = strlen(local.sun_path) + sizeof(local.sun_family);
  rc = bind(s, (struct sockaddr*)&local, len);
  if (rc < 0) return rc;

  rc = listen(s, 10);
  if (rc < 0) return rc;
  return s;
}

int connection(ds4_t* ds4, int fd) {
  int rc;
  ds4_shared_data_t shared_data;

  rc = recv(fd, &shared_data, sizeof(shared_data), 0);
  if (rc == 0) {
    close(fd);
    return;
  }
  if (rc != sizeof(shared_data)) {
    printf("ERROR: Unix Socket Only Recv %d bytes\n", rc); 
    close(fd);
    return -1;
  }

  if (shared_data.send_data) {
    if (shared_data.r + shared_data.g + shared_data.b != 0) {
      ds4_queue_rgb(ds4, shared_data.r, shared_data.g, shared_data.b);
    }
    if (shared_data.rumble) {
      ds4_queue_rumble(ds4);
    }
  }

  shared_data.rumble = 0;
  shared_data.send_data = 0;

  memcpy(&shared_data.controls, ds4_controls(ds4), sizeof(ds4_controls_t));

  rc = send(fd, &shared_data, sizeof(shared_data), MSG_NOSIGNAL);
  close(fd);
  if (rc != sizeof(shared_data)) {
    printf("ERROR: Unix Socket Only Sent %d bytes\n", rc); 
    return -1;
  }

  return 0;
}

int controller_connected_loop(ds4_t* ds4) {
  struct pollfd fds[2];
  int unix_fd, bt_fd;
  struct sockaddr_un remote;
  int t;
  int rc;
  struct timeval curtime;
  struct timeval last_ds4;

  unix_fd = setup_unix_socket();

  if (unix_fd < 0) {
    printf("ERROR: Could not setup unix socket\n");
    return unix_fd;
  }

  bt_fd = ds4_socket(ds4);

  if (bt_fd < 0) {
    printf("ERROR: Could not setup bt socket\n");
    close(unix_fd);
    return bt_fd;
  }

  fds[0].fd = unix_fd;
  fds[0].events = POLLIN;
  fds[1].fd = bt_fd;
  fds[1].events = POLLIN | POLLHUP | POLLERR;

  while (keep_running) {
    //POLL for 2 secs
    rc = poll(fds, 2, 2000);
    if (rc == -1) {
      printf("ERROR: poll failed\n");
      close(unix_fd);
      return rc;
    }
    if (rc == 0) {
      continue;
    }
    // If hit on unix socket
    if (fds[0].revents & POLLIN) {
      gettimeofday(&curtime, NULL);
      int msec = ((curtime.tv_sec - last_ds4.tv_sec) * 1000 + (curtime.tv_usec - last_ds4.tv_usec) / 1000);
      if (msec > 1000) {
        //printf("Control packet requested with stale ds4 data: %d ms old\n", msec);
        //close(unix_fd);
        //return -1;
      } else {
        // accept the connection and spin off thread
        int client = accept(unix_fd, (struct sockaddr*)&remote, &t);
        // might want to put this in its own thread
        rc = connection(ds4, client);
      }
    }
    
    if (fds[1].revents & POLLIN) {
      //read new controls
      rc = ds4_read(ds4);
      if (rc == 0) {
        printf("ERROR: DS4 Disconnected during read\n");
        close(unix_fd);
        return -1;
      }
      if (rc < 0) {
        printf("ERROR: Error during read\n");
        close(unix_fd);
        return -1;
      }
      rc = ds4_write(ds4);
      if (rc == 0) {
        printf("ERROR: DS4 Disconnected during write\n");
        close(unix_fd);
        return -1;
      }
      if (rc < 0) {
        printf("ERROR: Error during write\n");
        close(unix_fd);
        return -1;
      }
      gettimeofday(&last_ds4, NULL);
    } 
    if (fds[1].revents & POLLHUP || fds[1].revents & POLLERR) {
      printf("ERROR: DS4 disconnected\n");
      close(unix_fd);
      return -1;
    }
  }
  close(unix_fd);
}

int connect_to_ds4(ds4_t* ds4) {
  int num_ds4_found = -1;
  int ds4_conn_status = -1;

  do {
    num_ds4_found = ds4_scan(ds4);
  } while (num_ds4_found == 0);

  if (num_ds4_found < 0) {
    printf("ERROR: Error during DS4 Scanning: %s\n", strerror(errno));
    return -1;
  }

  ds4_conn_status = ds4_connect(ds4);
  if (ds4_conn_status < 0) {
    printf("Error: Error during DS4 connection\n");
    return -1;
  }
  if (ds4_conn_status == 0) {
    // Lost connection, try again
    ds4_disconnect(ds4);
    return -1;
  }
}

int event_loop(ds4_t* ds4) {
  keep_running = 1;
  while (keep_running) {
    // Scan until ds4 found
    while (keep_running && connect_to_ds4(ds4) == -1)
      ;
    if (!keep_running) return 0;

    printf("DS4 Connected\n");
    controller_connected_loop(ds4);
  }
  return 0;
}

int main() {
  ds4_t* ds4;
  set_handlers();

  ds4 = ds4_new();
  if (!ds4) {
    printf("ERROR: Failed to init ds4\n");
    return -1;
  }

  event_loop(ds4);
  ds4_destroy(&ds4);
  return 1;
}
