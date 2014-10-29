// This will connect to a linux DS4

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>
#include <manualcontrol.h>

#include <mraa/i2c.h>

static int keep_running = 1;

void intHandler(int dummy) {
  keep_running = 0;
}

void print_usage() {
  printf("ds4_connect\n");
}

int main(int argc, char** argv) {
  int num_found = 0;
  int ret;
  ds4_bt_t device;
  unsigned char data[11];
  ds4_controls_t* controls;
  uint8_t buffer[2048];

  mraa_i2c_context i2c;
  i2c = mraa_i2c_init(6);
  mraa_i2c_address(i2c, 0);
  

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
  signal(SIGINT, intHandler);
  while (keep_running) {
    read_from_ds4(&device, data, sizeof(data));
    controls = (ds4_controls_t*)(data + 2);
    ret = controller_data_to_control_command(controls, buffer);
    mraa_i2c_write(i2c, buffer, ret);
    usleep(5000);
  }
  signal(SIGINT, SIG_DFL);

  mraa_i2c_stop(i2c);
  disconnect_from_ds4(&device);

  return 0;
}
