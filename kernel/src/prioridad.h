#ifndef SRC_PRIORIDAD_H_
#define SRC_PRIORIDAD_H_

#include <estructuras.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "ready.h"

t_pcb* prioridades(t_list*);
void* mayor_prioridad(void*, void*);
bool tiene_mayor_prioridad(void*, void*);

#endif /* SRC_PRIORIDAD_H_ */
