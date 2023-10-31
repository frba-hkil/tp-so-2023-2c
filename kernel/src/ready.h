
#ifndef SRC_READY_H_
#define SRC_READY_H_

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>

extern t_list* lista_ready;

extern sem_t sem_lista_ready;

extern pthread_mutex_t mutex_lista_ready;




#endif /* SRC_READY_H_ */
