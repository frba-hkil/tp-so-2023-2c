#include "paginas.h"

void manejar_page_fault(t_paquete* paquete) {
	t_list* data = deserealizar_paquete(paquete);
	uint32_t *nro_pagina = (uint32_t*)list_get(data, 0);
	uint32_t *pid = (uint32_t*)list_get(data, 1);
	t_tabla_pagina *tp = buscar_tabla_proceso(*pid);
	int i = 0, cant_entradas = list_size(tp->entradas);
	while(i < cant_entradas) {
		t_entrada_tabla_pagina *etp = (t_entrada_tabla_pagina *)list_get(tp->entradas, i);
		if(etp->nro_pagina == *nro_pagina) {
			asignar_frame(etp, *pid);
		}
	}
}

void asignar_frame(t_entrada_tabla_pagina *etp, uint32_t pid) {
	for(int i = 0; i < cantidad_frames; i++) {
		if(!(bitarray_test_bit(bitmap_frames, i*sizeof(memoria_config->TAM_PAGINA)))){
			etp->nro_frame = i;
			queue_push(paginas_en_memoria, etp);
			bitarray_set_bit(bitmap_frames, i*sizeof(memoria_config->TAM_PAGINA));
			return;
		}
	}
	reemplazar_pagina(etp, pid);
}

void fifo(t_entrada_tabla_pagina *new_etp, uint32_t pid) {
	t_entrada_tabla_pagina *etp = queue_pop(paginas_en_memoria);
	etp->presente = false;

	if(etp->modificado) {
		uint32_t pid_reemplazado = buscar_pid_en_frame(etp->nro_frame);
		actualizar_swap(pid_reemplazado, etp->nro_pagina, memoria + etp->nro_frame);
	}
	new_etp->presente = true;

	t_paquete *paquete = recibir_paquete(sockets_m.s_fs);
	t_list *valores = deserializar_valores_enteros(paquete);
	int i, cant_valores = list_size(valores);
	for(i = 0; i < cant_valores; i++) {
		memcpy(memoria + etp->nro_frame + i * sizeof(uint32_t), (uint32_t*)list_get(valores, i), sizeof(uint32_t));
	}

	list_destroy_and_destroy_elements(valores, free);
	eliminar_paquete(paquete);
}

void lru(t_entrada_tabla_pagina *new_etp, uint32_t pid) {
	int cant_tablas = list_size(tablas_de_paginas), cant_entradas;
	t_tabla_pagina * _tp = (t_tabla_pagina*)list_get(tablas_de_paginas, 0);
	t_entrada_tabla_pagina* _etp = list_get(_tp->entradas, 0);
	t_entrada_tabla_pagina *etp_a_reemplazar = _etp;
	int64_t t_acceso = _etp->ultimo_acceso;

	for(int i = 0; i < cant_tablas; i++) {
		t_tabla_pagina* tp = list_get(tablas_de_paginas, i);
		cant_entradas = list_size(tp->entradas);
		for(int j = 0; j < cant_entradas; j++) {
			t_entrada_tabla_pagina *etp = (t_entrada_tabla_pagina *)list_get(tp->entradas, j);
			if(etp->ultimo_acceso < t_acceso) {
				t_acceso = etp->ultimo_acceso;
				etp_a_reemplazar = etp;
			}
		}
	}
	etp_a_reemplazar->presente = false;

	if(etp_a_reemplazar->modificado) {
		uint32_t pid_reemplazado = buscar_pid_en_frame(etp_a_reemplazar->nro_frame);
		actualizar_swap(pid_reemplazado, etp_a_reemplazar->nro_pagina, memoria + etp_a_reemplazar->nro_frame);
	}

	t_paquete *paquete = recibir_paquete(sockets_m.s_fs);
	t_list *valores = deserializar_valores_enteros(paquete);
	int i, cant_valores = list_size(valores);
	for(i = 0; i < cant_valores; i++) {
		memcpy(memoria + etp_a_reemplazar->nro_frame + i * sizeof(uint32_t), (uint32_t*)list_get(valores, i), sizeof(uint32_t));
	}

	list_destroy_and_destroy_elements(valores, free);
	eliminar_paquete(paquete);
}

uint32_t buscar_pid_en_frame(uint32_t nro_frame) {
	int cant_tablas = list_size(tablas_de_paginas), cant_entradas;

	for(int i = 0; i < cant_tablas; i++) {
		t_tabla_pagina* tp = (t_tabla_pagina* )list_get(tablas_de_paginas, i);
		cant_entradas = list_size(tp->entradas);
		for(int j = 0; j < cant_entradas; j++) {
			t_entrada_tabla_pagina *etp = (t_entrada_tabla_pagina *)list_get(tp->entradas, j);
			if(etp->nro_frame == nro_frame && etp->presente)
				return tp->pid;
		}
	}

	return 0;
}

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
		e->posicion_en_swap = 0;
		list_add(tp->entradas, e);
	}

	return tp;
}

void eliminar_estructuras(t_paquete* paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	uint32_t pid = *(uint32_t*)list_get(datos, 0);
	t_tabla_pagina *tp = buscar_tabla_proceso(pid);

	liberar_espacio_usuario(tp);
	eliminar_tabla_de_pagina(tp);
	//free_process_swap(pid);

	dictionary_remove_and_destroy(instrucciones_de_procesos, string_itoa(pid), eliminar_instrucciones);
	eliminar_paquete(paquete);
	paquete = crear_paquete(FINALIZACION_DE_PROCESO, buffer_vacio());
	agregar_a_paquete(paquete, &pid, sizeof(uint32_t));
	enviar_paquete(paquete, sockets_m.s_fs);
}

void retornar_nro_frame(t_paquete* paquete) {	//nro pagina y pid
	t_list *datos = deserealizar_paquete(paquete);
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
	bool same_pid(void *tp){
		t_tabla_pagina *_tp = (t_tabla_pagina *)tp;
		return _tp->pid == _pid;
	}
	t_tabla_pagina *tp_proceso = list_find(tablas_de_paginas, same_pid);


	return tp_proceso;
}

t_entrada_tabla_pagina *buscar_entrada(t_direccion_fisica* df, uint32_t pid) {

	t_tabla_pagina *tp_proceso = buscar_tabla_proceso(pid);
	bool pagina_presente(void *etp) {
		t_entrada_tabla_pagina * _etp = (t_entrada_tabla_pagina *) etp;
		return _etp->nro_frame == df->nro_frame && _etp->presente;
	}
	t_entrada_tabla_pagina *etp = list_find(tp_proceso->entradas, pagina_presente);

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
			//void* posicion_memoria = memoria + etp->nro_frame * sizeof(memoria_config->TAM_PAGINA);
			bitarray_clean_bit(bitmap_frames, etp->nro_frame * sizeof(memoria_config->TAM_PAGINA));
		}
	}
}

void eliminar_tabla_de_pagina(t_tabla_pagina *tp) {
	int i = 0, cant_entradas = list_size(tp->entradas);
	while(i < cant_entradas) {
		t_tabla_pagina *_tp = (t_tabla_pagina *) list_get(tablas_de_paginas, i);
		if(_tp == tp)
			break;
	}
	list_remove_and_destroy_element(tablas_de_paginas, i, eliminar_tabla_de_pagina_proceso);
	//dictionary_remove_and_destroy(tablas_de_paginas, string_itoa(tp->pid), eliminar_tabla_de_pagina_proceso);
}

void eliminar_tabla_de_pagina_proceso(void *tp) {
	t_tabla_pagina *_tp = (t_tabla_pagina *)tp;
	list_destroy_and_destroy_elements(_tp->entradas, free);
	free(tp);
}

void actualizar_swap(uint32_t pid, uint32_t nro_pagina, void* frame) { //pedir que actualize bloque/pagigna numero n de tal proceso
	//tengo que reescribir los datos en el frame donde se hizo el reemplazo
	int i, cant_valores = memoria_config->TAM_PAGINA / sizeof(uint32_t);
	t_list *valores = list_create();

	for(i = 0; i < cant_valores; i++) {
		list_add(valores, (uint32_t*)(frame + i * sizeof(uint32_t)));
	}
	t_paquete *paquete = serializar_valores_enteros(valores, ACTUALIZAR_VALORES_SWAP);
	agregar_a_paquete(paquete, &pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, &nro_pagina, sizeof(uint32_t));
	enviar_paquete(paquete, sockets_m.s_fs);
	eliminar_paquete(paquete);
	list_destroy_and_destroy_elements(valores, free);

}


