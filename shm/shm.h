#ifndef __ELEVATE_SHM__
#define __ELEVATE_SHM__
#include <sys/types.h> /* key_t */

typedef struct _shm_t shm_t;

// Two different contstructors
shm_t* shm_create(key_t key);
shm_t* shm_attach(key_t key);
void shm_destroy(shm_t** self_p);

int   shm_lock(shm_t* self);
int   shm_unlock(shm_t* self);

void* shm_data(shm_t* self);
int   shm_exists(shm_t* self);

int shm_detach(shm_t* self);
#endif
