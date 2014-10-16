#ifndef __DS4_CONTROLLER__
#define __DS4_CONTROLLER__
struct _controls {
  // byte 0
  int left_analog_x  : 8;

  // byte 1
  int left_analog_y  : 8;

  // byte 2
  int right_analog_x : 8;

  // byte 3
  int right_analog_y : 8;

  // byte 4
  unsigned dpad           : 3;
  unsigned _unknown       : 1;
  unsigned square         : 1;
  unsigned x              : 1;
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
};

typedef struct _controls controls_t;
#endif
