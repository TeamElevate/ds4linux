#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <shm.h>
#include <ds4_data.h>
#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  keep_running = 0;

  sigaction(sig, &sa, NULL);
}

int main() {
  int num_ds4_found;
  int ds4_conn_status;
  int bytes_read;
  int rc;
  const ds4_controls_t* controls;
  key_t key;
  ds4_shared_data_t* shared_data;

  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  shm_t* shm;


  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  ds4_t* ds4 = ds4_new();
  if (!ds4) {
    printf("ERROR: Failed to init ds4\n");
    return -1;
  }

  // Setup shared memory
  key = ftok("/opt/controller.ipc", 'R');
  if (key == -1)  {
    printf("ERROR: Failed to get key for shm\n");
    return -1;
  }

  shm = shm_create(key);
  if (!shm) {
    printf("ERROR: Failed to create shm\n");
    return -1;
  }

  shared_data = (ds4_shared_data_t*)shm_data(shm);
  shm_lock(shm);
  shared_data->controller_connected = 0;
  shm_unlock(shm);

  // Daemon
  // Scans then connects to a DS4
  keep_running = 1;
  while (keep_running) {
    // Scan until ds4 found
    do {
      num_ds4_found = ds4_scan(ds4);
    } while (num_ds4_found == 0);

    if (num_ds4_found < 0) {
      printf("ERROR: Error during DS4 Scanning: %s\n", strerror(errno));
      continue;
    }

    ds4_conn_status = ds4_connect(ds4);
    if (ds4_conn_status < 0) {
      printf("Error: Error during DS4 connection\n");
      continue;
    }
    if (ds4_conn_status == 0) {
      // Lost connection, try again
      ds4_disconnect(ds4);
      continue;
    }

    // Send and receive until disconnect 
    while (keep_running) {
      shm_lock(shm);
      if (shared_data->send_data) {
        uint8_t r, g, b, rumble;
        r      = shared_data->r;
        g      = shared_data->g;
        b      = shared_data->b;
        rumble = shared_data->rumble;
        shared_data->rumble = 0;
        shared_data->send_data = 0;
        shm_unlock(shm);
        printf("Sent RGB\n");
        rc = ds4_set_rgb(ds4, r, g, b);
        assert(rc > 0);
        if (rumble) {
          printf("Sent rumble\n");
          ds4_rumble(ds4);
        }
      } else {
        shm_unlock(shm);
      }
      if (ds4_peek(ds4)) {

        bytes_read = ds4_read(ds4);
        if (bytes_read == 79) {
          controls = ds4_controls(ds4);
          shm_lock(shm);
          shared_data->controller_connected = 1;
          memcpy(&(shared_data->controls), controls, sizeof(ds4_controls_t));
          shm_unlock(shm);
        } else if (bytes_read == -1) {
          shm_lock(shm);
          shared_data->controller_connected = 0;
          shm_unlock(shm);
          printf("Error Reading: %s\n", strerror(errno));
          break;
        } else if (bytes_read == 0) {
          shm_lock(shm);
          shared_data->controller_connected = 0;
          shm_unlock(shm);
          // Disconnect
          break;
        }
      }
    }
    shm_lock(shm);
    shared_data->controller_connected = 0;
    shm_unlock(shm);
    ds4_disconnect(ds4);
  }
  shm_detach(shm);
  shm_destroy(&shm);
}
