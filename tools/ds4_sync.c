// This will set the DS4's target bluetooth address (which is the address the
// DS4 will pair with) to either this computer's bluetooth MAC address or a
// specified MAC address.

#include <assert.h>
#include <stdio.h>

#include <ds4_usb.h>
#include <ds4_bt.h>

void print_usage() {
  printf("ds4_sync <mac_addr>\n");
  printf("If no mac_addr given, this computers bluetooth adapter's MAC is used\n");
}

int main(int argc, char** argv) {
  int r;
  unsigned char ds4_addr[18];
  unsigned char bd_addr[18];
  unsigned char* addr = NULL;
  ds4_usb_t ds4_usb;

  r = ds4_usb_init(&ds4_usb);
  if (r <= 0) {
    printf("No DS4 Controllers found\n");
    return -1;
  }

  assert(ds4_usb.devh);

  ds4_usb_get_mac(&ds4_usb, ds4_addr);
  printf("Current MAC: %s\n", ds4_addr);
  if (argc > 1) {
    addr = (unsigned char*)argv[1];
  } else {
    addr = bd_addr;
    get_bd_addr((char*)addr);
  }
  printf("My MAC: %s\n", addr);
  ds4_usb_set_mac(&ds4_usb, addr);
  ds4_usb_get_mac(&ds4_usb, addr);
  printf("New MAC: %s\n", addr);

  ds4_usb_deinit(&ds4_usb);

  return 0;

}
