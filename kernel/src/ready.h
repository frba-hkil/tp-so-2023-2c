
#ifndef SRC_READY_H_
#define SRC_READY_H_

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <estructuras.h>
#include "kernel_conexiones.h"
#include "syscall.h"

extern int plani_running;

extern t_queue* cola_blocked_sleep;

extern pthread_mutex_t mutex_cola_sleep;

extern t_list* lista_ready;

extern sem_t sem_lista_ready;

extern pthread_mutex_t mutex_lista_ready;

extern pthread_cond_t cond_ready_agregado;

extern t_dictionary *recursos_sistema;

extern bool bloqueado;

extern t_dictionary *colas_blocked;

extern pthread_mutex_t mutex_cola_blocked;

#endif /* SRC_READY_H_ */
