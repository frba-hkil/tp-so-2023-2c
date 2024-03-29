#ifndef SRC_TRANSICIONES_ESTADO_PCB_H_
#define SRC_TRANSICIONES_ESTADO_PCB_H_

#include <estructuras.h>
#include "kernel_conexiones.h"
#include "ready.h"

void pcb_new_a_ready(t_pcb* new_pcb);

void pcb_a_exit(t_pcb *_pcb);

char* estado_a_string(int estado);

#endif /* SRC_TRANSICIONES_ESTADO_PCB_H_ */
