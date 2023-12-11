#include "paginas.h"

t_tabla_pagina *crear_tp(uint32_t _pid, int cant_entradas) {
	t_tabla_pagina* tp = malloc(sizeof(t_tabla_pagina));
	t_entrada_tabla_pagina *e;

	tp->pid = _pid;
	tp->entradas = list_create();

	for(int i = 0; i < cant_entradas; i++) {
		e = malloc(sizeof(t_entrada_tabla_pagina));
		e->modificado = false;
		e->nro_frame = UINT32_MAX;
		e->nro_pagina = i;
		e->presente = false;
		e->ultimo_acceso = 0;
		list_add(tp->entradas, e);
	}

	return tp;
}

void eliminar_estructuras(t_paquete* paquete) {
	t_list *datos = deserializar_paquete(paquete);
	uint32_t pid = *(uint32_t*)list_get(datos, 0);
	t_tabla_pagina *tp = buscar_tabla_proceso(pid);

	liberar_espacio_usuario(tp);
	eliminar_tabla_de_pagina(tp);
	//free_process_swap(pid);

	dictionary_remove_and_destroy(instrucciones_de_procesos, string_itoa(pid), eliminar_instrucciones);
}

void retornar_nro_frame(t_paquete* paquete) {	//nro pagina y pid
	t_list *datos = deserializar_paquete(paquete);
	uint32_t pid = *(uint32_t*)list_get(datos, 0);
	uint32_t nro_pagina = *(uint32_t*)list_get(datos, 1);

	t_tabla_pagina *tp = buscar_tabla_proceso(pid);
	t_entrada_tabla_pagina *entrada;

	if(nro_pagina < list_size(tp->entradas)) {
		entrada = list_get(tp->entradas, nro_pagina);
		if(entrada->presente) {
			t_paquete *pack = crear_paquete(RET_FRAME, buffer_vacio());
			agregar_a_paquete(pack, &entrada->nro_frame, sizeof(uint32_t));
			enviar_paquete(pack, sockets_m.s_cpu);
			eliminar_paquete(pack);
		}
		else {
			enviar_mensaje("page fault", PAGE_FAULT, sockets_m.s_cpu);
		}
	}
	else {
		log_error(memoria_logger, "acceso invalido"); //no devuelvo nada. direcciones en instruccion seran correctas siempre
	}
	list_destroy_and_destroy_elements(datos, free);
	//return PAGE_FAULT;
}

t_tabla_pagina *buscar_tabla_proceso(uint32_t _pid) {
	t_tabla_pagina *tp_proceso = list_find(tablas_de_paginas, same_pid);

	bool same_pid(t_tabla_pagina *tp){
		return tp->pid == _pid;
	}

	return tp_proceso;
}

t_entrada_tabla_pagina *buscar_entrada(t_direccion_fisica* df, uint32_t pid) {

	t_tabla_pagina *tp_proceso = buscar_tabla_proceso(pid);
	t_entrada_tabla_pagina *etp = list_find(tp_proceso->entradas, pagina_presente);

	bool pagina_presente(t_entrada_tabla_pagina *etp) {
		return etp->nro_frame == df->nro_frame && etp->presente;
	}

	return etp;
}

void escribir_en_memoria(uint32_t *valor, t_direccion_fisica* df, uint32_t pid) {
	void* posicion_memoria = memoria + df->nro_frame * sizeof(memoria_config->TAM_PAGINA) + df->desplazamiento * sizeof(uint32_t);

	memcpy(posicion_memoria, valor, sizeof(uint32_t));
	t_entrada_tabla_pagina *etp = buscar_entrada(df, pid);

	etp->ultimo_acceso = temporal_gettime(timer_lru);
	etp->modificado = true;
}

uint32_t *leer_de_memoria(t_direccion_fisica* df, uint32_t pid) {
	void* posicion_memoria = memoria + df->nro_frame * sizeof(memoria_config->TAM_PAGINA) + df->desplazamiento * sizeof(uint32_t);

	uint32_t *valor = (uint32_t*) posicion_memoria;
	t_entrada_tabla_pagina *etp = buscar_entrada(df, pid);
	etp->ultimo_acceso = temporal_gettime(timer_lru);

	return valor;
}

void liberar_espacio_usuario(t_tabla_pagina* tp) {	//solo marca en memoria que estan libres, no borra el contenido

	int i, cant_entradas = list_size(tp->entradas);
	for(i = 0; i < cant_entradas; i++) {
		t_entrada_tabla_pagina *etp = list_get(tp->entradas, i);
		if(etp->presente) {
			void* posicion_memoria = memoria + etp->nro_frame * sizeof(memoria_config->TAM_PAGINA);
			bitarray_clean_bit(bitmap_frames, posicion_memoria);
		}
	}
}

void eliminar_tabla_de_pagina(t_tabla_pagina *tp) {
	dictionary_remove_and_destroy(tablas_de_paginas, string_itoa(tp->pid), eliminar_tabla_de_pagina_proceso);
}

void eliminar_tabla_de_pagina_proceso(t_tabla_pagina *tp) {
	list_destroy_and_destroy_elements(tp->entradas, free);
	free(tp);
}


