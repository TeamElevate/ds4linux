#ifndef __MATT_BLUETOOTH_WRAPPER__
#define __MATT_BLUETOOTH_WRAPPER__

#include <bluetooth/bluetooth.h> /* bdaddr_t */
#include <inttypes.h>            /* uint8_t  */

struct _ds4_bt {
  // @TODO: Add linked list of controllers later
  struct _ds4_bt* next;
  bdaddr_t addr;
  int ctl_socket, int_socket;
};
typedef struct _ds4_bt ds4_bt_t;

// Gets the addr of the bluetooth chip
// Len must be greater than 19
int get_bd_addr(char* addr);
int set_bd_key(char* addr, uint8_t* key);

int scan_for_ds4(ds4_bt_t* device);
int connect_to_ds4(ds4_bt_t* device);
int disconnect_from_ds4(ds4_bt_t* device);
int read_from_ds4(ds4_bt_t* device, unsigned char* buf, size_t len);
int control_ds4(ds4_bt_t* device, unsigned char* buf, size_t len);

#endif
