// This will connect to a linux DS4

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <ds4_usb.h>
#include <ds4_bt.h>
#include <ds4.h>
#include <manualcontrol.h>

#include <mraa/i2c.h>

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
  int ret;
  int i;
  ds4_bt_t device;
  unsigned char data[11];
  ds4_controls_t* controls;
  uint8_t buffer[2048];
  uint8_t* d = buffer;
  mraa_result_t result;

  mraa_i2c_context i2c;
  i2c = mraa_i2c_init(6);
  if (!i2c) {
    printf("Error: Could not init i2c\n");
    return -1;
  }

  result = mraa_i2c_address(i2c, 0);
  if (result != MRAA_SUCCESS) {
    mraa_result_print(result);
    return -1;
  }
  

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
  if (ret < 0) {
    printf("Error setting up DS4 controller\n");
    mraa_i2c_stop(i2c);
    disconnect_from_ds4(&device);
  }
  
  // read data
  signal(SIGINT, intHandler);
  while (keep_running) {
    d = buffer;
    read_from_ds4(&device, data, sizeof(data));
    controls = (ds4_controls_t*)(data + 4);
    ret = controller_data_to_control_command(controls, d);
    while (ret > 0) {
      result = mraa_i2c_write(i2c, d, (ret > 32) ? 32 : ret);
      d += (ret > 32) ? 32 : ret;
      ret = ret - 32;
      if (result != MRAA_SUCCESS) {
        mraa_result_print(result);
        mraa_i2c_stop(i2c);
        i2c = mraa_i2c_init(6);
        result = mraa_i2c_address(i2c, 4);
        break;
      }
      usleep(100000);
    }
    for (i = 0; i < 55; i++) {
      printf("%02x ", buffer[i]);
    }
    printf("\n");
    printf("Bytes: %d\n", 55);
  }

  mraa_i2c_stop(i2c);
  disconnect_from_ds4(&device);

  return 0;
}
