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
  ds4_bt_t device;
  unsigned char data[11];
  controls_t* controls;
  

  // Scan
  num_found = scan_for_ds4(&device);
  if (0 == num_found) {
    printf("DS4 Was not found\n");
    return -1;
  }

  // Connect
  connect_to_ds4(&device);

  // read data
  while (1) {
    read_from_ds4(&device, data, sizeof(data));
    controls = (controls_t*)(data + 2);
    printf("X: %1d Circle: %1d Triangle: %1d Square: %1d\n", controls->x, controls->circle, controls->triangle, controls->square);
    usleep(5000);
  }

  return 0;
}
