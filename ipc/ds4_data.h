#ifndef __ELEVATE_DS4_SHARED_DATA__
#define __ELEVATE_DS4_SHARED_DATA__
#include <inttypes.h>
#include <ds4.h>

const char* const DS4_KEY_FILE;

typedef struct {
  uint8_t        controller_connected;
  uint8_t        send_data;
  uint8_t        r;
  uint8_t        g;
  uint8_t        b;
  uint8_t        rumble;
  ds4_controls_t controls;
} ds4_shared_data_t;
#endif
