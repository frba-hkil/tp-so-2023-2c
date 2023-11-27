#include "gestionar_conexiones.h"

static void retardo_respuesta(uint32_t tiempo);


void procesar_conexiones(t_cliente *datos_cliente) {
	
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		// KERNEL
		case INICIALIZACION_DE_PROCESO:
			uint32_t tamanio_proceso = recibir_proceso(paquete);
			//log_info(memoria_logger, "Inicializando estructuras de PID[%d]...", pcb->id);
			//swap_crear_archivo(pcb->id, pcb->tamanio_proceso);
			//uint32_t tabla_primer_nivel = crear_tablas_de_paginacion(pcb->id);
			//enviar_direccion_tabla_primer_nivel(datos_cliente->socket, tabla_primer_nivel);

			//eliminar_pcb(pcb);
			break;

		case INSTRUCCION:
			enviar_instruccion(paquete, datos_cliente->socket);
		default:
			log_error(memoria_logger, "Protocolo invalido.");
			break;
	}
	//eliminar_paquete(paquete);
}

uint32_t recibir_proceso(t_paquete* paquete){
	t_list* datos = deserealizar_paquete(paquete);
	uint32_t pid = *(uint32_t *)list_get(datos, 0);
	uint32_t tamanio_proceso = *(uint32_t *)list_get(datos, 1);
	char* nombre_archivo = (char *)list_get(datos, 2);

	t_list* instrucciones = leer_instrucciones(nombre_archivo);

	dictionary_put(instrucciones_de_procesos, pid, instrucciones);

	list_destroy_and_destroy_elements(datos, free);

	return tamanio_proceso;
}

void enviar_instruccion(t_paquete* paquete, int socket_fd){
	t_list* datos = deserealizar_paquete(paquete);

	uint32_t pid = *(uint32_t *)list_get(datos, 0);
	uint32_t program_counter = *(uint32_t *)list_get(datos, 1);

	t_list* instrucciones = dictionary_get(instrucciones_de_procesos, pid);

	t_instruccion* instruccion = list_get(instrucciones, program_counter);

	t_paquete* paquete_instruccion = serializar_instruccion(instruccion, INSTRUCCION);

	enviar_paquete(paquete_instruccion, socket_fd);

	eliminar_paquete(paquete_instruccion);
}

void enviar_direccion_tabla_primer_nivel(int socket_fd, uint32_t direccion) {
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void informar_estado_proceso(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

void enviar_estructura_traductora(int socket_fd, t_traductor *traductor) {
	t_paquete *paquete = serializar_traductor(traductor, HANDSHAKE_INICIAL);
	retardo_respuesta(memoria_config->RETARDO_RESPUESTA);
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

void enviar_direccion_tabla_segundo_nivel(int socket_fd, uint32_t direccion) {
	retardo_respuesta(memoria_config->RETARDO_RESPUESTA);
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero) {
	retardo_respuesta(memoria_config->RETARDO_RESPUESTA);
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor) {
	retardo_respuesta(memoria_config->RETARDO_RESPUESTA);
	enviar_datos(socket_fd, &valor, sizeof(uint32_t));
}

void informar_escritura_realizada(int socket_fd, t_protocolo protocolo) {
	retardo_respuesta(memoria_config->RETARDO_RESPUESTA);
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}


static void retardo_respuesta(uint32_t tiempo) {
	usleep(tiempo * 1000);
}

