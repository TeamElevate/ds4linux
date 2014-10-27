#ifndef __DS4_CONTROLLER__
#define __DS4_CONTROLLER__
typedef struct {
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

  // The reset is  trackpad data... later

} ds4_controls_t;

#endif
