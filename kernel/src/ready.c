#include "ready.h"

int plani_running;

pthread_mutex_t mutex_lista_ready = PTHREAD_MUTEX_INITIALIZER;

t_list* lista_ready;

sem_t sem_lista_ready;

pthread_cond_t cond_ready_agregado;

t_queue* cola_blocked_sleep;

sem_t sem_sleep;

pthread_mutex_t mutex_cola_sleep = PTHREAD_MUTEX_INITIALIZER;

t_dictionary *recursos_sistema;

bool bloqueado;

t_dictionary *colas_blocked;

pthread_mutex_t mutex_cola_bloqueado = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_bloqueado = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_recursos_s = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_recursos_p = PTHREAD_MUTEX_INITIALIZER;

sem_t sem_check_deadlock;

sem_t retomar;

t_pcb *proceso_en_exec;

pthread_mutex_t mutex_proceso_en_exec = PTHREAD_MUTEX_INITIALIZER;

t_pcb *proceso_bloqueado_por_pf;
