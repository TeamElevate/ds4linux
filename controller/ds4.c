#include <assert.h>   /* assert */
#include <stdlib.h>   /* malloc, free */

#include "ds4.h"
#include <ds4_bt.h>

struct _ds4_t {
  ds4_bt_t* bt;
  ds4_controls_t controls;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t rumble;
  uint8_t write;
};

ds4_t* ds4_new() {
  ds4_t* self = malloc(sizeof(ds4_t));
  self->r      = 0xFF;
  self->g      = 0xFF;
  self->b      = 0xFF;
  self->bt     = 0x0;
  self->rumble = 0x0;
  self->write  = 0x0;
  return self;
}

void ds4_destroy(ds4_t** self_p) {
  assert(self_p);
  if (*self_p) {
    ds4_t* self = *self_p;
    if (self->bt) {
      ds4_bt_destroy(&self->bt);
    }
    free(self);
    *self_p = NULL;
  }
}

int ds4_scan(ds4_t* self) {
  int num_found;
  assert(self);
  if (!self->bt) {
    self->bt = ds4_bt_new();
    if (!self->bt) return -1;
  }
  num_found = ds4_bt_scan(self->bt);
  if (num_found < 0) {
    ds4_bt_destroy(&self->bt);
    return -1;
  }
  return num_found;
}

int ds4_connect(ds4_t* self) {
  assert(self);

  if (!self->bt) return -1;

  if (ds4_bt_connected(self->bt)) {
    return 0;
  }

  if (ds4_bt_connect(self->bt) != 0) {
    ds4_bt_destroy(&self->bt);
    return -1;
  }

  // So that we recieved correct packets back
  return ds4_set_rgb(self, self->r, self->g, self->b);
}

int ds4_disconnect(ds4_t* self) {
  assert(self);
  if (self->bt) {
    ds4_bt_destroy(&self->bt);
  }
  return 0;
}

int ds4_socket(ds4_t* self) {
  assert(self);
  assert(self->bt);
  return ds4_bt_handle(self->bt);
}

void ds4_queue_rgb(ds4_t* self, uint8_t r, uint8_t g, uint8_t b) {
  assert(self);
  self->write = self->write || (self->r != r) || (self->g != g) || (self->b != b);
  self->r = r;
  self->g = g;
  self->b = b;
}

void ds4_queue_rumble(ds4_t* self) {
  assert(self);
  self->write = 1;
  self->rumble = 0xFF;
}

int ds4_write(ds4_t* self) {
  int rc;
  uint8_t rgb[3];
  assert(self);
  if (!self->bt) return -1;
  if (!self->write) return 1;
  rgb[0] = self->r;
  rgb[1] = self->g;
  rgb[2] = self->b;
  rc = ds4_bt_write(self->bt, rgb, self->rumble);
  self->rumble = 0x0;
  self->write = 0x0;
  return rc;
}

int ds4_set_rgb(ds4_t* self, uint8_t r, uint8_t g, uint8_t b) {
  assert(self);
  ds4_queue_rgb(self, r, g, b);
  self->write = 0x1;
  return ds4_write(self);
}

int ds4_rumble(ds4_t* self) {
  assert(self);
  ds4_queue_rumble(self);
  return ds4_write(self);
}

int ds4_read(ds4_t* self) {
  int ret;
  unsigned char buffer[79];

  assert(self);
  ret = ds4_bt_read(self->bt, buffer, sizeof(buffer));
  if (ret == sizeof(buffer)) {
    memcpy(&self->controls, buffer + 4, sizeof(ds4_controls_t));
    self->controls.left_analog_x = htobs(self->controls.left_analog_x);
    self->controls.left_analog_y = 255 - htobs(self->controls.left_analog_y);
    self->controls.right_analog_x = htobs(self->controls.right_analog_x);
    self->controls.right_analog_y = htobs(self->controls.right_analog_y);
  }
  return ret;
}

const ds4_controls_t* ds4_controls(const ds4_t* self) {
  assert(self);
  return &self->controls;
}
