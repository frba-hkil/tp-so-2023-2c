#ifndef SRC_SYSCALL_H_
#define SRC_SYSCALL_H_

#include <estructuras.h>
#include <protocolo.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdlib.h>
#include "ready.h"

void atender_cpu(t_pcb* pcb, t_protocolo protocolo);

void atender_syscall(t_pcb* pcb);

void bloquear_por_sleep(t_pcb *pcb);

void bloquearse_por(void* pcb);

#endif /* SRC_SYSCALL_H_ */
