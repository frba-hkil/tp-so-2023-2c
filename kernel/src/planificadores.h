#ifndef SRC_PLANIFICADORES_H_
#define SRC_PLANIFICADORES_H_

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
//#include <serializador.h>
#include "transiciones_estado_pcb.h"
#include "prioridad.h"
#include "round_robin.h"
#include "ready.h"

sem_t sem_planificacion_l;
sem_t sem_planificacion_c;
//sem_t sem_admitir;
//sem_t sem_finalizar;
sem_t sem_grado_multiprogramacion;
sem_t sem_new;
//sem_t sem_lista_ready;

//int plani_running;

extern pthread_mutex_t mutex_plani_running;
pthread_cond_t cond_plani_running;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;

pthread_t thread_admitir;
pthread_t thread_finalizar;

//t_list *lista_ready;

void plani_largo_pl(void);

void plani_corto_pl(char*);

void admitir_procesos(void);

void finalizar_procesos(void);

void fifo(t_list*);

void atender_bloqueados(void);

#endif /* SRC_PLANIFICADORES_H_ */
