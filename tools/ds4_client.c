#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <ds4.h>
#include <shm.h>
#include <ds4_data.h>

volatile sig_atomic_t keep_running;

void intHandler(int sig) {
  keep_running = 0;
  signal(sig, SIG_DFL);
}

int main() {
  int len, ret;
  ds4_controls_t* controls;
  ds4_shared_data_t* shared_data;

  struct sigaction sa;
  sa.sa_handler = intHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  key_t key;
  shm_t* shm;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("ERROR: Could not set signal handler\n");
    return -1;
  }

  // Setup shared memory
  key = ftok("/opt/controller.ipc", 'R');
  if (key == -1)  {
    printf("ERROR: Failed to get key for shm\n");
    return -1;
  }
  shm = shm_attach(key);
  if (!shm) {
    printf("ERROR: Daemon not running");
    return -1;
  }

  shared_data = (ds4_shared_data_t*)shm_data(shm);
  if (!shared_data) {
    printf("Data is unavailable");
    return -1;
  }


  keep_running = 1;

  while (keep_running) {
    shm_lock(shm);
    if (shared_data->controller_connected) {
      controls = (ds4_controls_t*)&(shared_data->controls);
      printf("Cross: %1u\n", controls->cross);
    }
    shm_unlock(shm);
  }

  shm_detach(shm);

  return 0;
}
