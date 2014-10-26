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
  int ret;
  ds4_bt_t device;
  unsigned char data[11];
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

  // read data
  while (1) {
    read_from_ds4(&device, data, sizeof(data));
    controls = (ds4_controls_t*)(data + 2);
    printf("Left X: %d Left Y: %d Right X: %d Right Y: %d\n",
            controls->left_analog_x,
            controls->left_analog_y,
            controls->right_analog_x,
            controls->right_analog_y);
    usleep(5000);
  }

  return 0;
}
