#ifndef SRC_KERNEL_OPERACIONES_H_
#define SRC_KERNEL_OPERACIONES_H_

#include <stdio.h>
#include <stdint.h>
#include <estructuras.h>
#include <print.h>
#include "consola.h"


void (*command_handlers[6])(char**);


Comando codigo_consola;
char** parametros_consola;

pthread_mutex_t mutex_inst_consola;
sem_t sem_consola;
uint32_t generador_de_id;

char *op_strings[16];

void escuchar_consola(void);

void iniciar_proceso(char** parametros);

uint32_t crear_pid(void);

t_list *crear_instrucciones(FILE* proc);

t_op_code string_a_op_code(char* str);

#endif /* SRC_KERNEL_OPERACIONES_H_ */
