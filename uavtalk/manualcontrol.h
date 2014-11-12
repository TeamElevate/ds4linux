#ifndef __DS4LINUX_MANUAL_CONTROL__
#define __DS4LINUX_MANUAL_CONTROL__

#include <ds4.h>                 /* ds4_controls_t */
#include <inttypes.h>            /* uint8_t  */

uint16_t controller_data_to_control_command(const ds4_controls_t*, uint8_t*);

#endif
