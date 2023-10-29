#ifndef KERNEL_GLOBAL_H_
#define KERNEL_GLOBAL_H_

#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <protocolo.h>
#include <estructuras.h>
#include <serializador.h>
#include <conexiones.h>
#include <cliente.h>
#include "kernel_config.h"
#include "consola.h"
#include "print.h"

// Kernel
t_log *kernel_logger;
t_kernel_config *kernel_config;
int socket_cpu_interrupt;
int socket_cpu_dispatch;
int socket_memoria;
int sockets[4]; /*
sockets[0] = cpu_dispatch,
sockets[1] = cpu_interrupt,
sockets[2] = memoria,
sockets[3] = filesystem
*/

extern sem_t sem_consola; // para asegurar que se procese el comando de consola una vez que el usuaria haya ingresado algo
extern int plani_running;
extern sem_t sem_planificacion_l; // dispara senial para arrancar planificador de largo plazo
extern sem_t sem_planificacion_c; // dispara senial para arrancar planificador de corto plazo

// Planificador Largo Plazo
extern uint32_t generador_de_id; // simiplemente es un contador para registrar pids a los pcbs que se van generando
//uint32_t procesos_admitidos_en_ready;
t_queue *cola_new;
t_queue *cola_exit;
sem_t sem_admitir;
sem_t sem_exit;
sem_t sem_grado_multiprogramacion;
extern pthread_mutex_t mutex_inst_consola;
extern pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;
pthread_t thread_exit;
pthread_t thread_admitir;


// Planificador Corto Plazo
t_queue *cola_new;
extern t_list *cola_ready;
t_queue *cola_exec;
t_queue *cola_blocked;
sem_t sem_ready;
sem_t sem_exec;
sem_t sem_blocked;
sem_t sem_desalojo;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exec;
pthread_t thread_ready;
pthread_t thread_exec;
pthread_t thread_blocked;


// Planificador Mediano Plazo
t_queue *cola_suspended_blocked;
t_queue *cola_suspended_ready;
//sem_t sem_suspended_blocked;
sem_t sem_suspended_ready;
pthread_mutex_t mutex_suspended_blocked;
pthread_mutex_t mutex_suspended_ready;
pthread_t thread_suspended_ready;

pthread_t console_thread;

#endif /* KERNEL_GLOBAL_H_ */
