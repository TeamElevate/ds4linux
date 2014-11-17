#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <ds4.h>
#include <manualcontrol.h>
#include <crc.h>
#include <client.h>

#include <mraa/i2c.h>

// Returns time diff in terms of ms
static long get_time_diff(const struct timeval start, const struct timeval end) {
  return ((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) + 0.5;
}

static int send_UAVObj(mraa_i2c_context i2c, uint8_t* buf, int len) {
  int total = len;
  int to_send = 0;
  mraa_result_t result;

  uint8_t *crc = buf + len;
  *crc = updateCRC(0x00, buf, len);
  len++;

  while (len > 0) {
    to_send = (len > 32) ? 32 : len;
    result = mraa_i2c_write(i2c, buf, to_send);
    buf += to_send;
    len -= to_send;
    if (result != MRAA_SUCCESS) {
      mraa_result_print(result);
      return -1;
    }
  }
  return total;
}

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  keep_running = 0;
  // Set back to default handler
  sigaction(sig, &sa, NULL);
}

const int MAX_NUM_ERRS = 10;

int main(int argc, char** argv) {
  int rc;
  const ds4_controls_t* controls;
  uint8_t buffer[2048];
  mraa_result_t result;
  struct timeval start, end;
  mraa_i2c_context i2c;
  ds4_client_t* client;
  int numerrs;

  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  numerrs = 0;

  keep_running = 1;


  // Set Up SIG handler
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }
  while (keep_running) {
    if (numerrs > MAX_NUM_ERRS) {
      printf("ERROR: Too many errors. Exiting...\n");
      break;
    }
    /**
     * Initialize I2C
     */
    i2c = mraa_i2c_init(6);
    if (!i2c) {
      printf("Error: Could not init i2c\n");
      numerrs++;
      continue;
    }

    result = mraa_i2c_address(i2c, 0);
    if (result != MRAA_SUCCESS) {
      printf("ERROR: Couldn't set I2C address\n");
      mraa_result_print(result);
      mraa_i2c_stop(i2c);
      numerrs++;
      continue;
    }


    /**
     * Connect with DS4 daemon
     */
    client = ds4_client_new();
    if (!client) {
      mraa_i2c_stop(i2c);
      numerrs++;
      printf("ERROR: Can't find ds4 daemon: %s\n", strerror(errno));
      continue;
    }

    rc = ds4_client_attach(client);
    if (rc == -1) {
      ds4_client_destroy(&client);
      mraa_i2c_stop(i2c);
      numerrs++;
      printf("ERROR: Could not attach to daemon\n");
      continue;
    }
    


    // Wait till I2C comes online
    while (keep_running && mraa_i2c_write_byte(i2c, 0x00) != MRAA_SUCCESS) {
      sleep(1);
    }

    // @TODO: Set controller to correct COLOR (waiting for controller)

    // Wait for a DS4 controller
    while (keep_running && !ds4_client_is_controller_connected(client)) {
      sleep(1);
    }

    // @TODO: Set controller to correct COLOR going


    gettimeofday(&start, NULL);
    while (keep_running) {
      if (!ds4_client_is_controller_connected(client)) {
        continue;
      }
      controls = ds4_client_controls(client);
      gettimeofday(&end, NULL);
      
      if (get_time_diff(start, end) < 50) continue;
      gettimeofday(&start, NULL);
      
      rc = controller_data_to_control_command(controls, buffer);
      rc = send_UAVObj(i2c, buffer, rc);

      // If error while sending, try reinitizalizing
      if (rc == -1) {
        // Bad I2C
        //@TODO: ds4_set_rgb(ds4, 0xFF, 0x00, 0x00);
        numerrs++;
        break;
      }
      numerrs = 0;

      // @TODO:Set to Green if successful

      printf("Bytes: %d\n", rc);
    }
    mraa_i2c_stop(i2c);
    ds4_client_destroy(&client);
  }

  return 0;
}
