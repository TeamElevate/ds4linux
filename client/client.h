// Class to connect to DS4 daemon
#ifndef __ELEVATE_DS4_CLIENT__
#define __ELEVATE_DS4_CLIENT__
#include <ds4.h>

typedef struct _ds4_client_t ds4_client_t;

ds4_client_t* ds4_client_new();
void ds4_client_destroy(ds4_client_t** self_p);

int ds4_client_attach(ds4_client_t* self);
int ds4_client_is_controller_connected(ds4_client_t* self);
ds4_controls_t* ds4_client_controls(ds4_client_t* self);

int ds4_client_rgb(ds4_client_t* self, uint8_t r, uint8_t g, uint8_t b);
int ds4_client_rumble(ds4_client_t* self);
#endif
