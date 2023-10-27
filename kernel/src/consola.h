#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <readline/readline.h>
#include <readline/history.h>
#include <commons/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <semaphore.h>

#define CANT_MAX_COMANDOS 6

typedef enum{
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	DETENER_PLANIFICACION,
	INICIAR_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO
}Comando;

t_log *kernel_logger;
sem_t sem_consola;
pthread_mutex_t mutex_inst_consola;
extern Comando codigo_consola;
extern char** parametros_consola;

//char* comandos[CANT_MAX_COMANDOS] = {"iniciar_proceso", "finalizar_proceso", "detener_planificacion", "iniciar_planificacion", "multiprogramacion", "proceso_estado"};

void iniciar_consola(void);

char** console_handler(void);

void* console_routine(void*);

void interpretar_comando(char* input, char** parametros);

void print_param(char*);

char** copy_string_array(char** original, int size);

#endif /* SRC_CONSOLA_H_ */
