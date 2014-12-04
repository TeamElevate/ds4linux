// This will connect to a linux DS4

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <client.h>
#include <ds4.h>

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  keep_running = 0;

  sigaction(sig, &sa, NULL);
}

void set_handlers() {
  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    exit(-1);
  }
}

int main(int argc, char** argv) {
  int num_found = 0;
  int bytes_read;
  int ret;
  unsigned char data[79];
  const ds4_controls_t* controls;
  int remote_disconnect = 0;
  ds4_client_t* client = ds4_client_new();

  if (!ds4_client_connected(client)) {
    printf("ERROR: No client connected\n");
    return -1;
  }

  keep_running = 1;
  while (keep_running) {
    controls = ds4_client_controls(client);
    if (!controls) {
      printf("Disconnected\n");
      return -1;
    }
    ds4_client_rgb(client, controls->left_analog_y, controls->left_analog_x, controls->right_analog_y);
    if (controls->cross) {
      ds4_client_rumble(client);
    }
    printf("Left X: %6d Left Y: %6d Right X: %6d Right Y: %6d\n",
            controls->left_analog_x,
            controls->left_analog_y,
            controls->right_analog_x,
            controls->right_analog_y
          );
  }
  ds4_client_destroy(&client);
  return 0;
}
