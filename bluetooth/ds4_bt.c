#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include "ds4_bt.h"

#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13

#define REPORT_ID           0x11
#define REPORT_SIZE           11

static int  open_bt() {
  int dev_id, dd;

  dev_id = hci_get_route(NULL);
  assert(dev_id >= 0);
  dd = hci_open_dev( dev_id );
  assert(dd >= 0);

  return dd;
}

int get_bd_addr(char* addr) {
  int dd;
  int r;

  dd = open_bt();

  bdaddr_t my_addr;
  r = hci_read_bd_addr(dd, &my_addr, 0);
  assert(r == 0);
  ba2str(&my_addr, addr);

  close(dd);
  return 0;
}


int set_bd_key(char* addr, uint8_t* key) {
  int dd;
  int r;

  dd = open_bt();

  bdaddr_t bdaddr;
  str2ba(addr, &bdaddr);

  r = hci_write_stored_link_key(dd, &bdaddr, key, 0);
  assert(r == 0);
  return 0;
}

int scan_for_ds4(ds4_bt_t* device) {
  inquiry_info* ii = NULL;
  int max_rsp, num_rsp;
  int len, flags;
  int i;
  char name[248] = { '\0' };
  int dev_id, dd;
  int num_found = 0;

  dev_id = hci_get_route(NULL);
  assert(dev_id >= 0);
  dd = hci_open_dev( dev_id );
  assert(dd >= 0);

  len = 2;
  max_rsp = 255;
  flags = IREQ_CACHE_FLUSH;
  ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

  num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
  assert(num_rsp >= 0);

  for (i = 0; i < num_rsp; i++) {
    if (hci_read_remote_name(dd, &(ii+i)->bdaddr, sizeof(name), name, 0) >= 0) {
      if (strcmp("Wireless Controller", name) == 0) {
        bacpy(&device->addr, &(ii+i)->bdaddr);
        num_found++;
      }
    }
  }

  free(ii);
  close(dd);
  return num_found;
}

int connect_to_ds4(ds4_bt_t* device) {
  int ret;
  struct sockaddr_l2 ctl_addr = { 0 };
  struct sockaddr_l2 int_addr = { 0 };

  device->ctl_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
  device->int_socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

  ctl_addr.l2_family = AF_BLUETOOTH;
  ctl_addr.l2_psm    = htobs(L2CAP_PSM_HIDP_CTRL);
  bacpy(&ctl_addr.l2_bdaddr, &device->addr);
  int_addr.l2_family = AF_BLUETOOTH;
  int_addr.l2_psm    = htobs(L2CAP_PSM_HIDP_INTR);
  bacpy(&int_addr.l2_bdaddr, &device->addr);

  ret = connect(device->ctl_socket, (struct sockaddr *)&ctl_addr, sizeof(ctl_addr));
  if (ret != 0) {
    printf("Error in creating socket: %s\n", strerror(errno));
    return ret;
  }
  ret = connect(device->int_socket, (struct sockaddr *)&int_addr, sizeof(int_addr));
  assert(ret == 0);

  return 0;
}

int read_from_ds4(ds4_bt_t* device, unsigned char* buf, size_t len) {
  int bytes_read;

  bytes_read = read(device->int_socket, buf, len);

  return bytes_read;
}
