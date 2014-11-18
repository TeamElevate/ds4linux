// Class to connect to DS4 daemon
#ifndef __ELEVATE_DS4_CLIENT__
#define __ELEVATE_DS4_CLIENT__
#include <ds4.h>

typedef struct _ds4_client_t ds4_client_t;

/**
 * Create a new ds4 client
 *
 * Check for a null ptr on return!
 */
ds4_client_t* ds4_client_new();

/**
 * Destructor for DS4 client
 *
 * pass in address of ds4_client_t*
 */
void ds4_client_destroy(ds4_client_t** self_p);

/**
 * Check is a DS4 is currently connected
 * Returns 1 if connected, 0 if not
 */
int ds4_client_connected(ds4_client_t* self);

/**
 * Gets the latest ds4 controller data
 */
const ds4_controls_t* ds4_client_controls(ds4_client_t* self);

/**
 * Set the DS4 rgb and rumble
 *
 * Returns 0 on sucess, negative number on failure
 */
int ds4_client_rgb(ds4_client_t* self, uint8_t r, uint8_t g, uint8_t b);
int ds4_client_rumble(ds4_client_t* self);
#endif
