#include "gestionar_conexiones.h"

static void retardo_respuesta(uint32_t tiempo);

void atender_kernel(void) {
	pthread_create(&thread_kernel, NULL, (void*)escuchar_kernel, NULL);
}

void atender_cpu(void) {
	if(pthread_create(&thread_cpu, NULL, (void*)escuchar_cpu, NULL))
		log_info(memoria_logger, "no pudo crearse hilo cpu");
}

void atender_fs(void) {
	pthread_create(&thread_fs, NULL,(void*) escuchar_cpu, NULL);
}

void escuchar_kernel(void) {
	t_paquete* paquete;

	while(1) {
		paquete = recibir_paquete(sockets_m.s_kernel);

		switch(paquete->codigo_operacion) {

		case INICIALIZACION_DE_PROCESO:
			recibir_proceso(paquete);
			break;
		case FINALIZACION_DE_PROCESO:
			eliminar_estructuras(paquete);
			break;
		default:
			;

			eliminar_paquete(paquete);
		}
	}
}

void escuchar_cpu(void) {
	t_paquete* paquete;
	t_contexto_ejecucion *ce;

	paquete = crear_paquete(HANDSHAKE_INICIAL, buffer_vacio());
	agregar_a_paquete(paquete, &memoria_config->TAM_PAGINA, sizeof(int));
	enviar_paquete(paquete, sockets_m.s_cpu);
	eliminar_paquete(paquete);

	while(1) {
		ce = malloc(sizeof(t_contexto_ejecucion));

		paquete = recibir_paquete(sockets_m.s_cpu);
		//log_info(memoria_logger, "recibi solicitud cpu");
		switch(paquete->codigo_operacion) {

		case SOLICITAR_INSTRUCCION:
			deserializar_contexto_ejecucion(ce, paquete);
			eliminar_paquete(paquete);

			enviar_instruccion(ce, paquete);
			break;

		default:
			;
		}
		eliminar_contexto_ejecucion(ce);
	}
}

void recibir_proceso(t_paquete* paquete){
	t_list* datos = deserealizar_paquete(paquete);
	uint32_t pid = *(uint32_t *)list_get(datos, 0);
	uint32_t tamanio_proceso = *(uint32_t *)list_get(datos, 1);
	char* nombre_archivo_r = (char *)list_get(datos, 2), *nombre_archivo_a = string_duplicate(memoria_config->PATH_INSTRUCCIONES);

	string_append(&nombre_archivo_a, nombre_archivo_r);

	t_list* instrucciones = leer_instrucciones(nombre_archivo_a);

	dictionary_put(instrucciones_de_procesos, string_itoa(pid), instrucciones);

	list_destroy_and_destroy_elements(datos, free);

	int cant_paginas_proceso = ceil((double)(tamanio_proceso / memoria_config->TAM_PAGINA));
	t_tabla_pagina *tabla_pag_proceso = crear_tp(pid, cant_paginas_proceso);
	list_add(tablas_de_paginas, tabla_pag_proceso);

	//solicitar_espacio_swap(pid, cant_paginas_swap);
}

void enviar_instruccion(t_contexto_ejecucion* ce, t_paquete* paquete){
	t_list *instrucciones = (t_list*)dictionary_get(instrucciones_de_procesos, string_itoa(ce->pid));
	t_instruccion* inst = (t_instruccion*)list_get(instrucciones, ce->program_counter);
	//print_instrucciones(memoria_logger, instrucciones);
	paquete = serializar_instruccion(inst, INSTRUCCION);
	if(inst->identificador == MOV_IN || inst->identificador == MOV_OUT) {
		enviar_paquete(paquete, sockets_m.s_cpu);
		eliminar_paquete(paquete);
		paquete = recibir_paquete(sockets_m.s_cpu);
		retornar_nro_frame(paquete);
	}
	else {
		enviar_paquete(paquete, sockets_m.s_cpu);
	}
	eliminar_paquete(paquete);
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

