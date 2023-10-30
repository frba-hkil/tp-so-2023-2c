#ifndef SRC_PLANIFICADORES_H_
#define SRC_PLANIFICADORES_H_

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include "transiciones_estado_pcb.h"

sem_t sem_planificacion_l;
sem_t sem_planificacion_c;
//sem_t sem_admitir;
//sem_t sem_finalizar;
sem_t sem_grado_multiprogramacion;
sem_t sem_new;
sem_t sem_seguir_admitiendo;
sem_t sem_seguir_finalizando;

int plani_running;

extern pthread_mutex_t mutex_plani_running;
pthread_cond_t cond_plani_running;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;

pthread_t thread_admitir;
pthread_t thread_finalizar;

void plani_largo_pl(void);

void plani_corto_pl(void);

void admitir_procesos(void);


#endif /* SRC_PLANIFICADORES_H_ */
