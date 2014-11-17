#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <ds4.h>
#include <ds4_data.h>

#include "shm.h"

struct _shm_t {
  uint8_t creator;
  key_t key;
  int shmid;
  int semid;
  char* data;
  uint8_t locked;
};

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short int *array;
  struct seminfo *__buf;
};

static int shm_sem_init(int semid) {
  union semun argument;
  unsigned short values[1];
  values[0] = 1;
  argument.array = values;
  return semctl(semid, 0, SETALL, argument);
}

static shm_t* shm_create_and_attach(key_t key, uint8_t create) {
  int rc;
  shm_t* self = malloc(sizeof(shm_t));
  self->shmid = -1;
  self->semid = -1;
  if (!self) return NULL;

  int flags = 0644;
  if (create) {
    flags |= IPC_CREAT;
  }

  self->creator = create;

  self->shmid = shmget(key, sizeof(ds4_controls_t) + 1, flags);

  if (self->shmid == -1){
    free(self);
    return NULL;
  }

  self->semid = semget(key, 1, flags);

  if (self->semid == -1) {
    // Delete shared memory
    shmctl(self->shmid, IPC_RMID, NULL);
    free(self);
  }

  rc = shm_sem_init(self->semid);
  if (rc == -1) {
    // Delete shared memory
    shmctl(self->shmid, IPC_RMID, NULL);
    // Delete semaphore
    semctl(self->semid, 0 /*<- Ignored */, IPC_RMID, NULL);
    free(self);
  }
    


  self->data = (char*)shmat(self->shmid, (void*)0, 0);
  if (self->data == (void*)-1) {
    // Delete shared memory
    shmctl(self->shmid, IPC_RMID, NULL);
    // Delete semaphore
    semctl(self->semid, 0 /*<- Ignored */, IPC_RMID, NULL);
    free(self);
    return NULL;
  }
  memset(self->data, 0x0, sizeof(ds4_shared_data_t));
  self->locked = 0;
  return self;
}

shm_t* shm_create(key_t key) {
  return shm_create_and_attach(key, 1);
}

shm_t* shm_attach(key_t key) {
  return shm_create_and_attach(key, 0);
}

int shm_lock(shm_t* self) {
  int rc;
  struct sembuf operations[1];
  operations[0].sem_num = 0;
  operations[0].sem_op = -1;
  operations[0].sem_flg = SEM_UNDO;

  rc = semop(self->semid, operations, 1);
  assert(rc == 0);
  self->locked = 1;
  return rc;
}

int shm_unlock(shm_t* self) {
  int rc;
  struct sembuf operations[1];
  operations[0].sem_num = 0;
  operations[0].sem_op = 1;
  operations[0].sem_flg = SEM_UNDO;

  assert(self);
  assert(self->locked);

  rc = semop(self->semid, operations, 1);
  assert(rc == 0);
  self->locked = 0;
  return rc;
}

void shm_destroy(shm_t** self_p) {
  assert(self_p);
  if (*self_p) {
    shm_t* self = *self_p;
    shm_detach(self);
    if (self->locked) {
      shm_unlock(self);
    }
    if (self->creator) {
      shmctl(self->shmid, IPC_RMID, NULL);
      // Delete semaphore
      semctl(self->semid, 0 /*<- Ignored */, IPC_RMID, NULL);
    }
    *self_p = NULL;
  }
}

void* shm_data(shm_t* self) {
  assert(self);
  return self->data;
}

int shm_exists(shm_t* self) {
  char* buf;
  assert(self);
  buf = (char*)shmat(self->shmid, (void*)0, 0);
  return (buf != (void*)-1);
}

int shm_detach(shm_t* self) {
  int rc = 0;
  assert(self);
  if (self->data) {
    rc = shmdt(self->data);
  }
  return rc;
}
