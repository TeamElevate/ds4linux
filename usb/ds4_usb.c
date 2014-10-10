#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "ds4_usb.h"

static const int DS4_VENDOR_ID  = 0x054C;
static const int DS4_PRODUCT_ID = 0x05C4;


static int is_a_ds4(const struct libusb_device_descriptor* desc) {
  if (desc->idVendor != DS4_VENDOR_ID) {
    return 0;
  }
  if (desc->idProduct != DS4_PRODUCT_ID) {
    return 0;
  }
  return 1;
}

static void process_ds4(struct libusb_device* ds4_dev, int infnum, ds4_usb_t* ds4) {
  struct libusb_device_handle* devh;
  int r;
 
  r = libusb_open(ds4_dev, &devh);
  assert(r == 0);

  r = libusb_set_auto_detach_kernel_driver(devh, 1);
  assert(r == 0);

  r = libusb_claim_interface(devh, infnum);
  assert(r == 0);

  if (!ds4->devh) {
    ds4->devh   = devh;
    ds4->infnum = infnum;
    return;
  }

  /*
  show_mac(devh, infnum);
  get_bd_addr(msg, 20);
  printf("Our MAC: %s\n", msg);
  set_mac(devh, infnum, msg);
  show_mac(devh, infnum);

  */

  r = libusb_release_interface(devh, infnum);
  assert(r == 0);
  libusb_close(devh);
}

// Only works for PS4 usb device
static int iter_dev(libusb_device* dev, ds4_usb_t* ds4) {
  int found = 0;
  int i, j;
  int r;
  struct libusb_device_descriptor desc;
  struct libusb_config_descriptor* config;

  r = libusb_get_device_descriptor(dev, &desc);
  assert(r == 0);

  if (!is_a_ds4(&desc)) return 0;

  for (i = 0; i < desc.bNumConfigurations; i++) {
    libusb_get_config_descriptor(dev, i, &config);
    for (j = 0; j < config->bNumInterfaces; j++) {
      process_ds4(dev, i, ds4);
      found++;
    }
    libusb_free_config_descriptor(config);
  }

  return found;
}

static struct libusb_context* ctx = NULL;
static struct libusb_device **devs;

int ds4_usb_init(ds4_usb_t* ds4) {
  assert(ctx == NULL);
  int found = 0;
  int cnt = 0;
  int ret;
  ssize_t i;

  ds4->devh = NULL;
  ret = libusb_init(&ctx);
  assert(ret == 0);

  libusb_set_debug(ctx, 3);

  cnt = libusb_get_device_list(ctx, &devs);
  assert(cnt >= 0);

  for (i = 0; i < cnt; i++) {
    found += iter_dev(devs[i], ds4);
  }

  return found;
}

int ds4_usb_deinit(ds4_usb_t* ds4) {
  int r;

  assert(ctx != NULL);
  r = libusb_release_interface(ds4->devh, ds4->infnum);
  assert(r == 0);
  libusb_close(ds4->devh);
  libusb_free_device_list(devs, 1);
  libusb_exit(ctx);
  ctx = NULL;
  return 0;
}

int ds4_usb_set_mac(ds4_usb_t* ds4_usb, const unsigned char* mac) {
  unsigned char msg[23];
  int mac_bytes[6];
  assert(strlen((char*)mac) == 6);
  int r;

  msg[0] = 0x13;

  r = sscanf(
    (char*)mac,
    "%x:%x:%x:%x:%x:%x",
    &mac_bytes[0],
    &mac_bytes[1],
    &mac_bytes[2],
    &mac_bytes[3],
    &mac_bytes[4], 
    &mac_bytes[5]
  );

  assert(r == 6);

  msg[1] = (unsigned char)mac[5];
  msg[2] = (unsigned char)mac[4];
  msg[3] = (unsigned char)mac[3];
  msg[4] = (unsigned char)mac[2];
  msg[5] = (unsigned char)mac[1];
  msg[6] = (unsigned char)mac[0];
  msg[7]  = 0x56;
  msg[8]  = 0xE8;
  msg[9]  = 0x81;
  msg[10] = 0x38;
  msg[11] = 0x08;
  msg[12] = 0x06;
  msg[13] = 0x51;
  msg[14] = 0x41;
  msg[15] = 0xC0;
  msg[16] = 0x7F;
  msg[17] = 0x12;
  msg[18] = 0xAA;
  msg[19] = 0xD9;
  msg[20] = 0x66;
  msg[21] = 0x3C;
  msg[22] = 0xCE;

  r = libusb_control_transfer(
    ds4_usb->devh,
    LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
    0x09,
    0x0313,
    ds4_usb->infnum,
    msg,
    sizeof(msg),
    5000
  );

  assert(r == 23);
  return 0;
}

int ds4_usb_get_mac(ds4_usb_t* ds4_usb, unsigned char* mac) {
  unsigned char msg[16];
  int r;
  r = libusb_control_transfer(
    ds4_usb->devh,
    LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
    0x01,
    0x0312,
    ds4_usb->infnum,
    msg,
    sizeof(msg),
    5000
  );

  assert(r == sizeof(msg));

  r = sprintf((char*)mac, "%02x:%02x:%02x:%02x:%02x:%02x", msg[15], msg[14], msg[13], msg[12], msg[11], msg[10]);
  assert(r == 17);
  return 0;
}
