#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int get_bd_addr(char* addr, int len) {
  int dev_id, dd;

  dev_id = hci_get_route(NULL);
  assert(dev_id >= 0);
  dd = hci_open_dev( dev_id );
  assert(dd >= 0);

  assert(len >= 19);
  bdaddr_t my_addr;
  hci_read_bd_addr(dd, &my_addr, 0);
  ba2str(&my_addr, addr);

  close(dd);
  return 0;
}
