#include "estructuras.h"

t_instruccion *crear_instruccion(t_op_code op_code, char* primer_operando, char* segundo_operando) {
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	instruccion->identificador = op_code;
	instruccion->primer_operando = string_duplicate(primer_operando);
	instruccion->segundo_operando = string_duplicate(segundo_operando);

	return instruccion;
}

void eliminar_instrucciones(t_list *instrucciones) {
	list_destroy_and_destroy_elements(instrucciones, free);
}

void eliminar_contexto_ejecucion(t_contexto_ejecucion* ce) {
	free(ce->inst_desalojador->primer_operando);
	free(ce->inst_desalojador->segundo_operando);
	free(ce->inst_desalojador);
	free(ce->registros);
	free(ce);
}

t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, uint32_t prioridad) {
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->contexto = malloc(sizeof(t_contexto_ejecucion));
	pcb->contexto->registros = malloc(sizeof(t_registro));
	pcb->contexto->inst_desalojador = malloc(sizeof(t_instruccion));

	pcb->contexto->pid = id;
	pcb->contexto->inst_desalojador = crear_instruccion(EXIT, "\0","\0");
	pcb->tamanio_proceso = tamanio_proceso;
	pcb->contexto->program_counter = 0;
	pcb->prioridad = prioridad;
	pcb->estado = NEW;
	pcb->contexto->registros->AX = 0;
	pcb->contexto->registros->BX = 0;
	pcb->contexto->registros->CX = 0;
	pcb->contexto->registros->DX = 0;
	pcb->recursos_asignados = dictionary_create();


	return pcb;
}

void eliminar_pcb(t_pcb *pcb) {
	eliminar_contexto_ejecucion(pcb->contexto);
	dictionary_destroy(pcb->recursos_asignados);
	free(pcb);
}

void eliminar_recurso_asignado(void* _recurso) {
	t_recurso *recurso = (t_recurso*) _recurso;

	free(recurso->nombre);
	free(recurso);
}

t_traductor *crear_traductor_direcciones(int entradas_tabla, int tamanio_pagina) {
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = entradas_tabla;
	traductor->tamanio_pagina = tamanio_pagina;

	return traductor;
}

void eliminar_traductor_direcciones(t_traductor *traductor) {
	free(traductor);
}

t_tabla_acceso *crear_tabla_de_acceso(uint32_t direccion, uint32_t entrada) {
	t_tabla_acceso *tabla_acceso = malloc(sizeof(t_tabla_acceso));

	tabla_acceso->direccion = direccion;
	tabla_acceso->entrada = entrada;

	return tabla_acceso;
}

void eliminar_tabla_de_acceso(t_tabla_acceso *tabla_acceso) {
	free(tabla_acceso);
}


