#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>

static int keep_running = 1;

void intHandler(int sig) {
  keep_running = 0;
  signal(sig, SIG_DFL);
}

int main() {
  int num_ds4_found;
  int ds4_conn_status;
  int bytes_read;
  const ds4_controls_t* controls;

  ds4_t* ds4 = ds4_new();
  if (!ds4) {
    printf("ERROR: Failed to init ds4\n");
    return -1;
  }

  // Daemon
  // Scans then connects to a DS4
  while (keep_running) {
    // Scan until ds4 found
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
      // Lost connection
      ds4_disconnect(ds4);
      continue;
    }

    // Send and receive until disconnect 
    while (keep_running) {
      if (!ds4_peek(ds4)) continue;

      bytes_read = ds4_read(ds4);
      if (bytes_read == -1) {
        printf("Error Reading: %s\n", strerror(errno));
        break;
      } else if (bytes_read == 0) {
        break;
      } else if (bytes_read != 79) {
        continue;
      }
      controls = ds4_controls(ds4);
    }
    ds4_disconnect(ds4);
  }
}
