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

uint8_t key[16] = {
  0x56,
  0xE8,
  0x81,
  0x38,
  0x08,
  0x06,
  0x51,
  0x41,
  0xC0,
  0x7F,
  0x12,
  0xAA,
  0xD9,
  0x66,
  0x3C,
  0xCE
};

int main(int argc, char** argv) {
  int rc;
  unsigned i;
  unsigned char ds4_addr[18];
  unsigned char host_addr[18];
  unsigned char* addr = NULL;
  ds4_usb_t ds4_usb;
  uint8_t read_key[16];

  rc = ds4_usb_init(&ds4_usb);
  if (rc <= 0) {
    printf("No DS4 Controllers found\n");
  }

  assert(ds4_usb.devh);

  ds4_usb_get_mac(&ds4_usb, ds4_addr, host_addr);
  printf("Current MAC: %s\n", host_addr);
  printf("DS4 MAC: %s\n", ds4_addr);
  if (argc > 1) {
    addr = (unsigned char*)argv[1];
  } else {
    addr = host_addr;
    rc = get_bd_addr((char*)addr);
    if (rc < 0) {
      printf("Error: Unable to get MAC\n");
      ds4_usb_deinit(&ds4_usb);
      return -1;
    }
  }
  printf("Host MAC: %s\n", addr);
  ds4_usb_set_mac(&ds4_usb, addr, key);
  ds4_usb_get_mac(&ds4_usb, ds4_addr, host_addr);
  printf("New MAC: %s\n", host_addr);
  ds4_usb_deinit(&ds4_usb);

  // Set key
  rc = set_bd_key((char*)ds4_addr, key);
  if (rc < 0) {
    printf("Error: Unable to set link key\n");
    return -1;
  }
  printf("Setting Key:");
  for (i = 0; i < sizeof(key); i++) {
    printf(" %2x", key[i]);
  }
  printf("\n");

  return 0;

}
