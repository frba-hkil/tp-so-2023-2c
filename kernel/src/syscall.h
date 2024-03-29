#ifndef SRC_SYSCALL_H_
#define SRC_SYSCALL_H_

#include <estructuras.h>
#include <protocolo.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdlib.h>
#include "ready.h"

void atender_cpu(t_pcb* pcb, t_protocolo);

void atender_syscall(t_pcb* pcb);

void bloquear_por_sleep(t_pcb *pcb);

void bloquearse_por(void* pcb);

void wait(t_pcb* pcb);

void signal(t_pcb* pcb);

void liberar_recursos_asignados(t_pcb* pcb);

void liberar_recurso(char* _key, void* n);

#endif /* SRC_SYSCALL_H_ */
