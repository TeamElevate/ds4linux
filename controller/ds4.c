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
};

ds4_t* ds4_new() {
  ds4_t* self = malloc(sizeof(ds4_t));
  self->r  = 0xFF;
  self->g  = 0xFF;
  self->b  = 0xFF;
  self->bt = 0x0;
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

int ds4_connect(ds4_t* self) {
  assert(self);

  if(self->bt) return 0;
  self->bt = ds4_bt_new();
  if (ds4_bt_scan(self->bt) <= 0) {
    ds4_bt_destroy(&self->bt);
    return -2;
  }
  if (ds4_bt_connect(self->bt) != 0) {
    ds4_bt_destroy(&self->bt);
    return -1;
  }
  // So that we recieved correct packets back
  ds4_set_rgb(self, self->r, self->g, self->b);
  return 0;
}

int ds4_disconnect(ds4_t* self) {
  assert(self);
  if (self->bt) {
    ds4_bt_destroy(&self->bt);
  }
  return 0;
}

int ds4_set_rgb(ds4_t* self, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t rgb[3];
  assert(self);
  if (!self->bt) return -1;
  self->r = r;
  self->g = g;
  self->b = b;
  rgb[0] = self->r;
  rgb[1] = self->g;
  rgb[2] = self->b;
  return ds4_bt_write(self->bt, rgb, 0);
}

int ds4_rumble(ds4_t* self) {
  uint8_t rgb[3];
  assert(self);
  if (!self->bt) return -1;

  rgb[0] = self->r;
  rgb[1] = self->g;
  rgb[2] = self->b;
  return ds4_bt_write(self->bt, rgb, 0xFF);
}

int ds4_read(ds4_t* self) {
  int ret;
  unsigned char buffer[79];

  assert(self);
  ret = ds4_bt_read(self->bt, buffer, sizeof(buffer));
  if (ret == sizeof(buffer)) {
    memcpy(&self->controls, buffer + 4, sizeof(ds4_controls_t));
  }
  return ret;
}

const ds4_controls_t* ds4_controls(const ds4_t* self) {
  assert(self);
  return &self->controls;
}
