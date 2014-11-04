// This will connect to a linux DS4

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>

static int keep_running = 1;

void intHandler(int dummy) {
  keep_running = 0;
  signal(SIGINT, SIG_DFL);
}

void print_usage() {
  printf("ds4_connect\n");
}

int main(int argc, char** argv) {
  int num_found = 0;
  int bytes_read;
  int ret;
  ds4_bt_t device;
  unsigned char data[79];
  ds4_controls_t* controls;
  int remote_disconnect = 0;
  

  // Scan
  num_found = scan_for_ds4(&device);
  if (0 == num_found) {
    printf("DS4 Was not found\n");
    return -1;
  }

  // Connect
  ret = connect_to_ds4(&device);
  if (ret != 0) {
    printf("Error Connecting to DS4 controller\n");
    return -2;
  }

  // Set up ds4 to send us all info
  ret = control_ds4(&device, NULL, 0);

  // read data
  signal(SIGINT, intHandler);
  while (keep_running) {
    bytes_read = read_from_ds4(&device, data, sizeof(data));
    if (bytes_read == 0) {
      // Connection closed
      disconnect_from_ds4(&device);
      printf("Controller disconnected\n");
      remote_disconnect = 1;
      break;
    } else if (bytes_read != 79) {
      continue;
    }
    controls = (ds4_controls_t*)(data + 4);
    printf("Left X: %6d Left Y: %6d Right X: %6d Right Y: %6d\n",
            controls->left_analog_x,
            controls->left_analog_y,
            controls->right_analog_x,
            controls->right_analog_y
          );
  }
  if (!remote_disconnect) {
    disconnect_from_ds4(&device);
  }

  return 0;
}
