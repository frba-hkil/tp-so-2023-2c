#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#define CANT_MAX_COMANDOS 6

typedef enum{
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	DETENER_PLANIFICACION,
	INICIAR_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO
}Comando;

//char* comandos[CANT_MAX_COMANDOS] = {"iniciar_proceso", "finalizar_proceso", "detener_planificacion", "iniciar_planificacion", "multiprogramacion", "proceso_estado"};

void iniciar_consola(void);

void console_handler(void);

void* console_routine(void*);

void ejecutar_comando(char* input, char** parametros);



#endif /* SRC_CONSOLA_H_ */
