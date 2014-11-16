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
  keep_running = 0;
  signal(sig, SIG_DFL);
}

int main() {
  int len, rc;
  ds4_controls_t* controls;
  ds4_client_t* client;

  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  client = ds4_client_new();
  if (!client) {
    printf("Could not initialize client\n");
    return -1;
  }

  rc = ds4_client_attach(client);
  if (rc == -1) {
    printf("Could not attach to running daemon\n");
    return -1;
  }

  keep_running = 1;

  while (keep_running) {
    if (ds4_client_is_controller_connected(client)) {
      controls = ds4_client_controls(client);
      printf("Cross: %1u\n", controls->cross);
    }
  }

  ds4_client_destroy(&client);

  return 0;
}
