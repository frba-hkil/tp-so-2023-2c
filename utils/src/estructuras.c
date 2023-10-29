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

t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, t_list *instrucciones, uint32_t program_counter, uint32_t prioridad) {
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->contexto = malloc(sizeof(t_contexto_ejecucion));
	pcb->contexto->registros = malloc(sizeof(t_registro));

	pcb->contexto->pid = id;
	pcb->tamanio_proceso = tamanio_proceso;
	pcb->contexto->instrucciones = list_duplicate(instrucciones);//list_duplicate(instrucciones);
	pcb->contexto->program_counter = program_counter;
	pcb->prioridad = prioridad;

	return pcb;
}

void eliminar_pcb(t_pcb *pcb) {
	eliminar_instrucciones(pcb->contexto->instrucciones);
	free(pcb);
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


