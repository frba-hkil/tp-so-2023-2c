#include "ready.h"

int plani_running;

pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;

t_list* lista_ready;

sem_t sem_lista_ready;

pthread_cond_t cond_ready_agregado;

t_queue* cola_blocked_sleep;

pthread_mutex_t mutex_cola_sleep = PTHREAD_MUTEX_INITIALIZER;
