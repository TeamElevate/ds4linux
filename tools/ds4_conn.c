// This will connect to a linux DS4

#include <assert.h>
#include <errno.h>
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
  unsigned char data[79];
  const ds4_controls_t* controls;
  int remote_disconnect = 0;

  ds4_t* ds4 = ds4_new();

  // scan for ds4
  ret = ds4_scan(ds4);
  if (ret == 0) {
    printf("No DS4 Controllers found\n");
    return -1;
  } else if (ret < 0) {
    printf("Error while scanning\n");
    return -1;
  }

  // Connect to for one found
  ret = ds4_connect(ds4);
  if (-1 == ret) {
    printf("Error Connecting to DS4 controller\n");
    return -1;
  } else if (ret == 0) {
    printf("Error: Lost connection to DS4 controller\n");
    ds4_disconnect(ds4);
    return -1;
  }

  // read data
  signal(SIGINT, intHandler);
  while (keep_running) {
    bytes_read = ds4_read(ds4);
    if (bytes_read == -1) {
      printf("Error Reading: %s\n", strerror(errno));
      break;
    } else if (bytes_read == 0) {
      // Connection closed
      printf("Controller disconnected\n");
      break;
    } else if (bytes_read != 79) {
      continue;
    }
    controls = ds4_controls(ds4);
    ds4_set_rgb(ds4, controls->left_analog_y, controls->left_analog_x, controls->right_analog_y);
    if (controls->cross) {
      ds4_rumble(ds4);
    }
    printf("Left X: %6d Left Y: %6d Right X: %6d Right Y: %6d\n",
            controls->left_analog_x,
            controls->left_analog_y,
            controls->right_analog_x,
            controls->right_analog_y
          );
  }
  ds4_disconnect(ds4);
  return 0;
}
