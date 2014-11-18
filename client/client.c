#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <ds4_data.h>

#include "client.h"

struct _ds4_client_t {
  int sock;
  ds4_shared_data_t shared_data;
};

static int daemon_connect(ds4_client_t* self) {
  struct sockaddr_un remote;
  int len;
  int rc;

  self->sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (self->sock < 0) {
    return self->sock;
  }

  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, "/opt/controller.ipc");
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  rc = connect(self->sock, (struct sockaddr*)&remote, len);
  if (rc < 0) {
    close(self->sock);
    self->sock = -1;
    return -1;
  }
  return 0;
}

ds4_client_t* ds4_client_new() {
  ds4_client_t* self = malloc(sizeof(ds4_client_t));

  // Init to 0
  memset(self, 0x0, sizeof(ds4_client_t));
  if (!self) return NULL;

  // Get key
  self->sock = -1;

  return self;
}

void ds4_client_destroy(ds4_client_t** self_p) {
  assert(self_p);
  if (*self_p) {
    ds4_client_t* self = *self_p;
    if (self->sock != -1) {
      close(self->sock);
    }
    free(self);
    *self_p = NULL;
  }
}

ds4_controls_t* ds4_client_controls(ds4_client_t* self) {
  int rc;

  assert(self);
  rc = daemon_connect(self);
  if (rc < 0) {
    return NULL;
  }

  self->shared_data.send_data = 0;
  rc = send(self->sock, &self->shared_data, sizeof(ds4_shared_data_t), 0);

  if (rc != sizeof(ds4_shared_data_t)) {
    close(self->sock);
    self->sock = -1;
    return NULL;
  }

  rc = recv(self->sock, &self->shared_data, sizeof(ds4_shared_data_t), 0);

  if (rc != sizeof(ds4_shared_data_t)) {
    close(self->sock);
    self->sock = -1;
    return NULL;
  }
  return &self->shared_data.controls;
}

int ds4_client_rgb(ds4_client_t* self, uint8_t r, uint8_t g, uint8_t b) {
  int rc;
  assert(self);

  // @TODO

  return 0;
}

int ds4_client_rumble(ds4_client_t* self) {
  assert(self);

  // @TODO

  return 0;
}
