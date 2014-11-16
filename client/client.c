#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ds4_data.h>
#include <shm.h>

#include "client.h"

struct _ds4_client_t {
  key_t  key;
  shm_t* shm;
  void*  data;
  ds4_shared_data_t shared_data;
};

static void ds4_client_update(ds4_client_t* self) {
  assert(self);
  assert(self->data);
  shm_lock(self->shm);
  memcpy(&self->shared_data, self->data, sizeof(ds4_shared_data_t));
  shm_unlock(self->shm);
}

ds4_client_t* ds4_client_new() {
  ds4_client_t* self = malloc(sizeof(ds4_client_t));
  if (!self) return NULL;

  // Get key
  self->key = ftok(DS4_KEY_FILE, 'R');
  if (self->key == -1)  {
    free(self);
    return NULL;
  }

  return self;
}

void ds4_client_destroy(ds4_client_t** self_p) {
  assert(self_p);
  if (*self_p) {
    ds4_client_t* self = *self_p;
    shm_destroy(&self->shm);
    free(self);
    *self_p = NULL;
  }
}

int ds4_client_attach(ds4_client_t* self) {
  assert(self);
  // Attach to memory
  self->shm = shm_attach(self->key);
  if (!self->shm) {
    return -1;
  }

  self->data = shm_data(self->shm);
  if (!self->data) {
    shm_destroy(&self->shm);
    return -1;
  }
  return 0;
}

int ds4_client_is_controller_connected(ds4_client_t* self) {
  assert(self);
  ds4_client_update(self);
  return (self->shared_data.controller_connected == 1);
}

ds4_controls_t* ds4_client_controls(ds4_client_t* self) {
  assert(self);
  ds4_client_update(self);
  return &self->shared_data.controls;
}
