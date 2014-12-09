#ifndef __DS4_USB__
#define __DS4_USB__

#include <libusb-1.0/libusb.h>

struct _ds4_usb {
  // @TODO: Add linked list of controllers later
  struct _ds4_usb* next;
  libusb_device_handle* devh;
  int infnum;
};

typedef struct _ds4_usb ds4_usb_t;

/**
 * Searches for DS4s on USB. Returns number found.
 * Places the first one in the param
 */
int ds4_usb_init(ds4_usb_t*);

/**
 * Close the DS4 controller
 */
int ds4_usb_deinit(ds4_usb_t*);

/**
 * Set the MAC address for the DS4 to connect on regular pairing mode
 */
int ds4_usb_set_mac(ds4_usb_t*, const unsigned char*, const uint8_t*);

/**
 * Get the MAC address the DS4 will connect to on regular pairing mode
 */
int ds4_usb_get_mac(ds4_usb_t*, unsigned char*, unsigned char*);
#endif
