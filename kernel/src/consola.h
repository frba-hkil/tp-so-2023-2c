#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
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


void iniciar_consola(void);

#endif /* SRC_CONSOLA_H_ */
