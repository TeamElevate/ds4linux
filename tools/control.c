// This will connect to a linux DS4

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
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

static long get_time_diff(const struct timeval start, const struct timeval end) {
  return ((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) + 0.5;
}

int main(int argc, char** argv) {
  int bytes_read;
  int num_found = 0;
  int num_sent = 0;
  int ret;
  int i;
  unsigned char data[11];
  const ds4_controls_t* controls;
  uint8_t buffer[2048];
  uint8_t* d = buffer;
  mraa_result_t result;
  struct timeval start, end;
  long seconds, useconds; 

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

  // Set to Green
  ds4_set_rgb(ds4, 0x00, 0xFF, 0x00);

  gettimeofday(&start, NULL);
  
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
    gettimeofday(&end, NULL);
    if (get_time_diff(start, end) > 10) {
      gettimeofday(&start, NULL);
      ret = controller_data_to_control_command(controls, d);
      printf("Raw: %d\n", controls->right_analog_y);
      printf("Thrust: %f\n", (controls->right_analog_y > 127) ? (controls->right_analog_y - 127.0f) / 128.0f : 0.0f);
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
      }
      for (i = 0; i < 55; i++) {
        printf("%02x ", buffer[i]);
      }
      num_sent++;
      printf("\n");
      printf("Bytes: %d\n", 55);
      printf("Num Sent: %d\n", num_sent);
    }
  }

  mraa_i2c_stop(i2c);
  ds4_destroy(&ds4);

  return 0;
}
