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
		case PAGE_FAULT:
			manejar_page_fault(paquete);
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

	uint32_t cant_paginas_proceso = ceil((double)(tamanio_proceso / memoria_config->TAM_PAGINA));
	t_tabla_pagina *tabla_pag_proceso = crear_tp(pid, cant_paginas_proceso);
	list_add(tablas_de_paginas, tabla_pag_proceso);

	solicitar_espacio_swap(pid, cant_paginas_proceso);
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

void solicitar_espacio_swap(uint32_t pid, uint32_t cantidad_paginas_proceso) {
	t_paquete *paquete = crear_paquete(INICIALIZACION_DE_PROCESO, buffer_vacio());

	agregar_a_paquete(paquete, &pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, &cantidad_paginas_proceso, sizeof(uint32_t));
	enviar_paquete(paquete, sockets_m.s_fs);
	eliminar_paquete(paquete);

	paquete = recibir_paquete(sockets_m.s_fs);
	t_list *valores = deserializar_valores_enteros(paquete); //posiciones de bloques en swap

	t_tabla_pagina *tp = buscar_tabla_proceso(pid);

	int i = 0, cant_entradas = list_size(tp->entradas);
	while(i < cant_entradas) {
		t_entrada_tabla_pagina* etp = (t_entrada_tabla_pagina*)list_get(tp->entradas, i);
		etp->posicion_en_swap = *(uint32_t*)list_get(valores, i);
	}
	list_destroy_and_destroy_elements(valores, free);
}

static void retardo_respuesta(uint32_t tiempo) {
	usleep(tiempo * 1000);
}

