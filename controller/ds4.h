#ifndef __DS4_CONTROLLER__
#define __DS4_CONTROLLER__
#include <inttypes.h>   /* uint8_t */

typedef struct _ds4_t ds4_t;
typedef struct _ds4_controls_t ds4_controls_t;

/**
 * Create a new DS4 object
 */
ds4_t* ds4_new();

/**
 * Destroy a DS4 object. Close connections if they are open
 */
void ds4_destroy(ds4_t** self_p);

/**
 * Scan for a DS4 controller
 */
int ds4_scan(ds4_t* self);

/**
 * Connect to a DS4 controller
 * Must wait until ds4_scan returns
 */
int ds4_connect(ds4_t* self);

/**
 * Disconnect from DS4
 */
int ds4_disconnect(ds4_t* self);

/**
 * Queue up a change to the RGB led
 * Will be written on next ds4_write call
 */
void ds4_queue_rgb(ds4_t* self, uint8_t r, uint8_t g, uint8_t b);

/**
 * Queue up a rumble
 * Will be written on next ds4_write call
 */
void ds4_queue_rumble(ds4_t* self);

/**
 * Write latest queued RGB and rumble
 * Resets rumble until rumble is called again
 */
int ds4_write(ds4_t* self);

/**
 * Queue and write the LED rgb in the same command
 */
int ds4_set_rgb(ds4_t* self, uint8_t r, uint8_t g, uint8_t b);

/**
 * Queue and write the controller rumble in the same command
 */
int ds4_rumble(ds4_t* self);

/**
 * Returns latest read socket for DS4 or -1 if not connected
 */
int ds4_socket(ds4_t* self);

/**
 * Read the latest data from the controller
 */
int ds4_read(ds4_t* self);

/**
 * Get the last valid controller data
 */
const ds4_controls_t* ds4_controls(const ds4_t* self);


// Layout of controller data
struct _ds4_controls_t {
  // byte 0
  unsigned left_analog_x  : 8;

  // byte 1
  unsigned left_analog_y  : 8;

  // byte 2
  unsigned right_analog_x : 8;

  // byte 3
  unsigned right_analog_y : 8;

  // byte 4
  unsigned dpad           : 3;
  unsigned _unknown       : 1;
  unsigned square         : 1;
  unsigned cross          : 1;
  unsigned circle         : 1;
  unsigned triangle       : 1;

  // byte 5
  unsigned l1             : 1;
  unsigned r1             : 1;
  unsigned l2             : 1;
  unsigned r2             : 1;
  unsigned share          : 1;
  unsigned options        : 1;
  unsigned l3             : 1;
  unsigned r3             : 1;

  // byte 6
  unsigned ps_btn         : 1;
  unsigned track_pad_btn  : 1;
  unsigned timestamp      : 6;

  // byte 7
  unsigned l2_analog      : 8;

  // byte 8 
  unsigned r2_analog      : 8;

  // byte 9 - 10
  unsigned counter        : 16;

  // byte 11
  unsigned battery        : 8;


  // byte 12 - 13
  int roll      : 16;

  // byte 14 - 15
  int pitch     : 16;

  // byte 16 - 17
  int yaw       : 16;

  // byte 18 - 19
  int accel_x        : 16;

  // byte 20 - 21
  int accel_y        : 16;

  // byte 22 - 23
  int accel_z        : 16;

  // The rest is  trackpad data... later

};

#endif
