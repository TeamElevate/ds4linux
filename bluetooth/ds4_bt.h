#ifndef __MATT_BLUETOOTH_WRAPPER__
#define __MATT_BLUETOOTH_WRAPPER__

#include <bluetooth/bluetooth.h> /* bdaddr_t */
#include <inttypes.h>            /* uint8_t  */

typedef struct _ds4_bt ds4_bt_t;

/**
 * Construct a new DS4 bluetooth object
 */
ds4_bt_t* ds4_bt_new();

/**
 * Destruct a DS4 bluetooth object
 *
 * This function close any open bt sockets
 */
void ds4_bt_destroy(ds4_bt_t** self_p);

/**
 * Scan for DS4 controllers
 */
int ds4_bt_scan(ds4_bt_t* self);

/**
 * Connect to DS4 controller
 *
 * Only run after a scan
 */
int ds4_bt_connect(ds4_bt_t* self);

/**
 * Disconnect from a DS4 controller
 * Closes all bluetooth sockets
 */
int ds4_bt_disconnect(ds4_bt_t* self);

/**
 * Check for new data from DS4 controller
 */
int ds4_bt_peek(ds4_bt_t* self);

/**
 * Read len bytes or less from the DS4. The data is placed into the buffer.
 */
int ds4_bt_read(ds4_bt_t* self, unsigned char* buf, size_t len);

/**
 * Write to the DS4 controller. Control the RGB LED and if Rumbling
 */
int ds4_bt_write(ds4_bt_t* self, uint8_t rgb[3], uint8_t rumble);

/**
 * Get the bluetooth socket used for reading from the DS4
 */
int ds4_bt_handle(ds4_bt_t* self);

/**
 * Check if DS4 controller is connected
 */
int ds4_bt_connected(const ds4_bt_t* self);


/**
 * Helpful bluetooth functionality
 */
int get_bd_addr(char* addr);
int set_bd_key(const char* addr, uint8_t* key);
int del_bd_key(const char* addr);

#endif
