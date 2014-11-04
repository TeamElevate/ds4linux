// This will connect to a linux DS4

#include <assert.h>
#include <errno.h>
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
  int bytes_read;
  int num_found = 0;
  int ret;
  int i;
  unsigned char data[11];
  const ds4_controls_t* controls;
  uint8_t buffer[2048];
  uint8_t* d = buffer;
  mraa_result_t result;

  ds4_t* ds4 = ds4_new();

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
  

  // Warning: This could block for a little
  ret = ds4_connect(ds4);
  if (-2 == ret) {
    printf("DS4 Was not found\n");
    return -1;
  }
  if (-1 == ret) {
    printf("Error Connecting to DS4 controller\n");
    return -2;
  }

  // Set to Green
  ds4_set_rgb(ds4, 0x00, 0xFF, 0x00);
  
  // read data
  signal(SIGINT, intHandler);
  while (keep_running) {
    d = buffer;
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
    ret = controller_data_to_control_command(controls, d);
    while (ret > 0) {
      result = mraa_i2c_write(i2c, d, (ret > 32) ? 32 : ret);
      d += (ret > 32) ? 32 : ret;
      ret = ret - 32;
      if (result != MRAA_SUCCESS) {
        mraa_result_print(result);
        mraa_i2c_stop(i2c);
        i2c = mraa_i2c_init(6);
        mraa_i2c_address(i2c, 4);
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
  ds4_destroy(&ds4);

  return 0;
}
