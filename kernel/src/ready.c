#include "ready.h"

pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;

t_list* lista_ready;

sem_t sem_lista_ready;

pthread_cond_t cond_ready_agregado;
