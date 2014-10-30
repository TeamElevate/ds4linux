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
  int i;
  unsigned char data[11];

  ds4_controls_t controls;
  controls.left_analog_x  = 0.0;
  controls.right_analog_x = 0.0;
  controls.left_analog_y  = 0.0;
  controls.right_analog_y = 0.0;
  uint8_t buffer[2048];

  mraa_i2c_context i2c;
  i2c = mraa_i2c_init(6);
  mraa_i2c_address(i2c, 0);
  

  // read data
  signal(SIGINT, intHandler);
  while (keep_running) {
    ret = controller_data_to_control_command(&controls, buffer);
    mraa_i2c_write(i2c, buffer, ret);
    usleep(5000);
  }
  signal(SIGINT, SIG_DFL);

  mraa_i2c_stop(i2c);

  return 0;
}
