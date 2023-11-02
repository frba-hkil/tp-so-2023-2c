#ifndef SRC_PRIORIDAD_H_
#define SRC_PRIORIDAD_H_

#include <estructuras.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "ready.h"
#include "kernel_global.h"

void prioridades(t_list*);
void* mayor_prioridad(void*, void*);
void check_higher_prio(t_pcb*);

#endif /* SRC_PRIORIDAD_H_ */
