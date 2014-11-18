#ifndef __MATT_BLUETOOTH_WRAPPER__
#define __MATT_BLUETOOTH_WRAPPER__

#include <bluetooth/bluetooth.h> /* bdaddr_t */
#include <inttypes.h>            /* uint8_t  */

typedef struct _ds4_bt ds4_bt_t;

ds4_bt_t* ds4_bt_new();
void ds4_bt_destroy(ds4_bt_t** self_p);

int ds4_bt_scan(ds4_bt_t* self);
int ds4_bt_connect(ds4_bt_t* self);
int ds4_bt_disconnect(ds4_bt_t* self);
int ds4_bt_peek(ds4_bt_t* self);
int ds4_bt_read(ds4_bt_t* self, unsigned char* buf, size_t len);
int ds4_bt_write(ds4_bt_t* self, uint8_t rgb[3], uint8_t rumble);
int ds4_bt_handle(ds4_bt_t* self);

int ds4_bt_connected(const ds4_bt_t* self);

// Gets the addr of the bluetooth chip
// Len must be greater than 19
int get_bd_addr(char* addr);
int set_bd_key(char* addr, uint8_t* key);

#endif
