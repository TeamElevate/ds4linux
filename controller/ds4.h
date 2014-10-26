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
} ds4_controls_t;

#endif
