#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdint.h>

typedef enum {

	INICIALIZACION_DE_PROCESO = 1,		// Protocolo que envia Kernel a Memoria para indicarle que cree las estructuras necesarias y devuelva el numero de tabla de pagina
	SUSPENSION_DE_PROCESO,			// Protocolo que envia Kernel a Memoria para indicar que se debe liberar el espacio de memoria del procesos y escribir en SWAP
	FINALIZACION_DE_PROCESO,		// Protocolo que envia Kernel a Memoria para indicar que se debe liberar la memoria y eliminar el archivo SWAP
	PROCESO_SUSPENDIDO,				// Protocolo que envia Memoria a Kernel para indicar que se libero la memoria del proceso
	PROCESO_FINALIZADO,				// Protocolo que envia Memoria a Kernel para indicar que se libero la memoria y se elimino el archivo SWAP

	PCB,							// Protocolo que usan Kernel, CPU y Memoria e indica el envio de la estructura de PCB
	CONTEXTO_EJECUCION,				// Protocolo que usan Kernel, CPU y Memoria e indica el envio de la estructura contexto de ejecucion de la PCB

	DESALOJAR_PROCESO,          	// Protocolo que envia Kernel a CPU para indicar que desaloje el proceso en ejecucion
	DESALOJO_POR_IO,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por ejecutar IO
	DESALOJO_POR_EXIT,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por ejecutar EXIT
	DESALOJO_POR_IRQ,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por interrupccion
	DESALOJO_POR_SYSCALL,			// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado para realizar una syscall
	DESALOJO_PAGE_FAULT,			// Protocolo que envia CPU a Kernel para indicar que hubo page fault y hay que tratarlo

	SOLICITAR_INSTRUCCION,  		// Protocolo que envia CPU a Memoria para solicitar la siguiente instruccion a ejecutar
	INSTRUCCION,  					// Protocolo que envia Memoria a CPU la siguiente instruccion a ejecutar
	NO_INSTRUCCION,					// Protocolo que envia Memoria a CPU para indicar que no hay mas instrucciones

	RET_FRAME,						// Protocolo que envia Memoria a MMU que indica que encontro el frame
	PAGE_FAULT,						// Protocolo que envia Memoria a MMU que indica que no se encontro el frame
	PAGE_SIZE,						//Protocolo que envia CPU/MMU a Memoria para pedir el tamanio de pagina para calcular el nro de pagina
	ACCESO_TP,						//Protocolo que envia CPU/MMU a Memoria para pedir el nro de frame
	HANDSHAKE_INICIAL,				// Protocolo que envia CPU a Memoria e indica el envio de la configuracion para traducir direcciones logicas a fisicas
	LEER_MEMORIA,					// Protocolo que envia CPU/MMU a Memoria para pedir el valor almacenado en memoria
	ESCRIBIR_MEMORIA,				// Protocolo que envia CPU/MMU a Memoria para escribir en memoria un valor
	MEMORIA_MODIFICADA_OK			// Protocolo que envia Memoria a CPU/MMU para indicar que se escribio en memoria
} t_protocolo;

#endif /* PROTOCOLO_H_ */
