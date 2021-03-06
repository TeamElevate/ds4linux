#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <ds4.h>
#include <ds4_data.h>
#include <client.h>

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  keep_running = 0;

  sigaction(sig, &sa, NULL);
}

int main() {
  int len, rc;
  uint8_t cross;
  const ds4_controls_t* controls;
  ds4_client_t* client;

  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  cross = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  client = ds4_client_new();
  if (!client) {
    printf("Could not initialize client\n");
    return -1;
  }

  if (!ds4_client_connected(client)) {
    printf("No DS4 Connected\n");
    return -1;
  }

  keep_running = 1;

  while (keep_running) {
    controls = ds4_client_controls(client);
    if (keep_running && !controls) {
      printf("ERROR: Could not obtain controls\n");
      return -1;
    }
    if (cross != controls->cross) {
      cross = controls->cross;
      printf("Cross: %d\n", cross);
    }
  }

  ds4_client_destroy(&client);

  return 0;
}
