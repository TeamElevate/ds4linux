// This will set the DS4's target bluetooth address (which is the address the
// DS4 will pair with) to either this computer's bluetooth MAC address or a
// specified MAC address.

#include <assert.h>
#include <stdio.h>

#include <ds4_usb.h>
#include <ds4_bt.h>

int main() {
  int r;
  unsigned char addr[20];
  ds4_usb_t ds4_usb;
  get_bd_addr((char*)addr);

  r = ds4_usb_init(&ds4_usb);
  if (r <= 0) {
    printf("No DS4 Controllers found\n");
    return -1;
  }

  assert(ds4_usb.devh);

  ds4_usb_get_mac(&ds4_usb, addr);
  printf("Current MAC: %s\n", addr);

  ds4_usb_deinit(&ds4_usb);

  return 0;

}
