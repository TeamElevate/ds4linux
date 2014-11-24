#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "ds4_bt.h"

#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13

#define HIDP_TRANS_SET_REPORT  0x50
#define HIDP_DATA_RTYPE_OUTPUT 0x02

#define REPORT_ID           0x11
#define REPORT_SIZE           11

struct _ds4_bt {
  // @TODO: Add linked list of controllers later
  struct _ds4_bt* next;
  bdaddr_t addr;
  int ctl_socket, int_socket;
};

static int  open_bt() {
  int dev_id, dd;

  dev_id = hci_get_route(NULL);
  assert(dev_id >= 0);
  dd = hci_open_dev( dev_id );
  assert(dd >= 0);

  return dd;
}

ds4_bt_t* ds4_bt_new() {
  ds4_bt_t* self = malloc(sizeof(ds4_bt_t));
  self->next = NULL;
  self->ctl_socket = -1;
  self->int_socket = -1;
  return self;
}

void ds4_bt_destroy(ds4_bt_t** self_p) {
  assert(self_p);
  if (*self_p) {
    ds4_bt_t* self = *self_p;
    if (self->int_socket != -1) {
      ds4_bt_disconnect(self);
    }
    assert(self->next == NULL);
    free(self);
    *self_p = NULL;
  }
}


int ds4_bt_scan(ds4_bt_t* self) {
  inquiry_info* ii = NULL;
  int max_rsp, num_rsp;
  int len, flags;
  int i;
  char name[248] = { '\0' };
  int dev_id, dd;
  int num_found = 0;

  assert(self);

  dev_id = hci_get_route(NULL);
  assert(dev_id >= 0);
  dd = hci_open_dev( dev_id );
  assert(dd >= 0);

  len = 2;
  max_rsp = 255;
  flags = IREQ_CACHE_FLUSH;
  ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

  num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
  if (num_rsp < 0) {
    return -1;
  }

  // @TODO: There has to be a better way of finding the device
  for (i = 0; i < num_rsp; i++) {
    if (hci_read_remote_name(dd, &(ii+i)->bdaddr, sizeof(name), name, 0) >= 0) {
      if (strcmp("Wireless Controller", name) == 0) {
        bacpy(&self->addr, &(ii+i)->bdaddr);
        num_found++;
      }
    }
  }

  free(ii);
  close(dd);
  return num_found;
}

int ds4_bt_connect(ds4_bt_t* self) {
  int ret;
  char* data;
  struct sockaddr_l2 ctl_addr = { 0 };
  struct sockaddr_l2 int_addr = { 0 };
  assert(self);

  if (ds4_bt_connected(self)) {
    return 0;
  }

  self->ctl_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
  if (self->ctl_socket == -1) return -1;
  self->int_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
  if (self->int_socket == -1) {
    self->ctl_socket = -1;
    return -1;
  }

  ctl_addr.l2_family = AF_BLUETOOTH;
  ctl_addr.l2_psm    = htobs(L2CAP_PSM_HIDP_CTRL);
  data = (char*)memcpy(&ctl_addr.l2_bdaddr, &self->addr, sizeof(bdaddr_t));
  data = data;
  int_addr.l2_family = AF_BLUETOOTH;
  int_addr.l2_psm    = htobs(L2CAP_PSM_HIDP_INTR);
  bacpy(&int_addr.l2_bdaddr, &self->addr);

  ret = connect(self->ctl_socket, (struct sockaddr *)&ctl_addr, sizeof(ctl_addr));
  if (ret != 0) {
    ds4_bt_disconnect(self);
    printf("Error in creating ctl socket: %s\n", strerror(errno));
    return ret;
  }

  ret = connect(self->int_socket, (struct sockaddr *)&int_addr, sizeof(int_addr));
  if (ret != 0) {
    ds4_bt_disconnect(self);
    printf("Error in creating int socket: %s\n", strerror(errno));
    return ret;
  }

  return 0;
}

int ds4_bt_disconnect(ds4_bt_t* self) {
  assert(self);
  if (ds4_bt_connected(self)) {
    close(self->ctl_socket);
    close(self->int_socket);
  }
  self->ctl_socket = -1;
  self->int_socket = -1;
  return 0;
}

int ds4_bt_handle(ds4_bt_t* self) {
  return self->int_socket;
}

int ds4_bt_peek(ds4_bt_t* self) {
  struct pollfd ufds[1];
  int ret;
  if (self->int_socket == -1) return -1;

  ufds[0].fd = self->int_socket;
  ufds[0].events = POLLIN;

  ret = poll(ufds, 1, 0);

  return ufds[0].revents & POLLIN;
}

int ds4_bt_read(ds4_bt_t* self, unsigned char* buf, size_t len) {
  int bytes_read;
  assert(self);

  bytes_read = read(self->int_socket, buf, len);

  return bytes_read;
}

// @TODO: Update this to work much better
int ds4_bt_write(ds4_bt_t* self, uint8_t rgb[3], uint8_t rumble) {
  int bytes_written;
  unsigned char buf[79];
  memset(buf, 0, 79);
  buf[0] = HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_OUTPUT;
  buf[1] = 0x11;   /* report id */
  buf[2] = 0x80;   /* no idea why */
  buf[4] = 0xFF;   /* no idea why */

  buf[7] = rumble; /* right rumble */
  buf[8] = rumble; /* left rumble */

  buf[9]  = rgb[0]; // r
  buf[10] = rgb[1]; // g
  buf[11] = rgb[2]; // b

  bytes_written = write(self->ctl_socket, buf, sizeof(buf));
  return bytes_written;
}

int ds4_bt_connected(const ds4_bt_t* self) {
  assert(self);
  return self->ctl_socket != -1;
}

int get_bd_addr(char* addr) {
  int dd;
  int r;

  dd = open_bt();

  bdaddr_t my_addr;
  r = hci_read_bd_addr(dd, &my_addr, 0);

  ba2str(&my_addr, addr);
  close(dd);
  return r;
}


int set_bd_key(char* addr, uint8_t* key) {
  int dd;
  int r;

  dd = open_bt();

  bdaddr_t bdaddr;
  str2ba(addr, &bdaddr);

  r = hci_write_stored_link_key(dd, &bdaddr, key, 0);
  close(dd);
  return r;
}

int num_bd_key(char* addr) {
  int dd;
  int rc;

  bdaddr_t bdaddr;
  str2ba(addr, &bdaddr);

  dd = open_bt();
  rc = hci_read_stored_link_key(dd, &bdaddr, 0, 0);
  close(dd);
  return rc;
}
