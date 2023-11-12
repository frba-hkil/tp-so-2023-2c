#ifndef SRC_KERNEL_OPERACIONES_H_
#define SRC_KERNEL_OPERACIONES_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <estructuras.h>
#include <print.h>
#include <commons/collections/queue.h>
#include "consola.h"
#include "planificadores.h"
#include "kernel_config.h"

t_kernel_config *kernel_config;
void (*command_handlers[6])(char**);

//int plani_running;
Comando codigo_consola;
char** parametros_consola;

pthread_t plani_largo_thread;
pthread_t plani_corto_thread;

pthread_mutex_t mutex_plani_running;
pthread_mutex_t mutex_new;
sem_t sem_consola;
sem_t sem_new;
uint32_t generador_de_id;

t_queue *cola_new;
t_queue *cola_exit;

char *op_strings[16];

void escuchar_consola(void);

void iniciar_proceso(char** parametros);

void finalizar_proceso(char** parametros);

void iniciar_planificacion(char** parametros);

void detener_planificacion(char** parametros);

void cambiar_multiprogramacion(char** parametros);

t_list *crear_instrucciones(FILE* proc);

t_op_code string_a_op_code(char* str);

#endif /* SRC_KERNEL_OPERACIONES_H_ */
