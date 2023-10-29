#ifndef SRC_PLANIFICADORES_H_
#define SRC_PLANIFICADORES_H_

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>

sem_t sem_planificacion_l;
sem_t sem_planificacion_c;
int plani_running;

extern pthread_mutex_t mutex_plani_running;

void plani_largo_pl(void);

void plani_corto_pl(void);


#endif /* SRC_PLANIFICADORES_H_ */
