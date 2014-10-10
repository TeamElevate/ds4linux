#ifndef __DS4_USB__
#define __DS4_USB__

#include <libusb.h>

struct _ds4_usb {
  // @TODO: Add linked list of controllers later
  struct ds4_usb* next;
  libusb_device_handle* devh;
  int infnum;
};

typedef struct _ds4_usb ds4_usb_t;

// Search for DS4 Controller over USB
// Returns number found
// @TODO: Deal with more than one
int ds4_usb_init(ds4_usb_t*);
int ds4_usb_deinit(ds4_usb_t*);
int ds4_usb_set_mac(ds4_usb_t*, const unsigned char*);
int ds4_usb_get_mac(ds4_usb_t*, unsigned char*);
#endif
