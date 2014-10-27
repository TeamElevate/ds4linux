// This will connect to a linux DS4

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>

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

  // Set to send a lot of data
  ret = control_ds4(&device, NULL, 0);

  // read data
  while (1) {
    bytes_read = read_from_ds4(&device, data, sizeof(data));
    if (bytes_read == 0) {
      // Connection closed
      disconnect_from_ds4(&device);
      printf("Controller disconnected\n");
      break;
    } else if (bytes_read != 79) {
      continue;
    }
    controls = (ds4_controls_t*)(data + 4);
    printf("Accel X: %6d Accel Y: %6d Accel Z: %6d Roll: %6d Pitch: %6d Yaw: %6d\n",
            controls->accel_x,
            controls->accel_y,
            controls->accel_z,
            controls->roll,
            controls->pitch,
            controls->yaw
          );
  }

  return 0;
}
