#include "print.h"

void print_instrucciones(t_log *logger, t_list *instrucciones) {
	log_info(logger, "************ INTRUCCIONES ***********");
	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		log_info(logger, "ID: %d, OP_1: %s, OP_2: %s",
				instruccion->identificador, instruccion->primer_operando, instruccion->segundo_operando);
	}
	log_info(logger, "***********************************");
}

void print_pcb(t_log *logger, t_pcb *pcb) {
	log_info(logger, "**************** PCB **************");
	log_info(logger, "PID = %d", pcb->contexto->pid);
	log_info(logger, "Estado = %d", pcb->estado);
	log_info(logger, "Tamanio proceso = %d", pcb->tamanio_proceso);
	//log_info(logger, "Cantidad de instrucciones = %d", list_size(pcb->contexto->instrucciones));
	log_info(logger, "Program counter = %d", pcb->contexto->program_counter);
	log_info(logger, "Prioridad = %d", pcb->prioridad);
	//log_info(logger, "Tabla de paginas = %d", pcb->tabla_paginas);
	//log_info(logger, "Estimacion rafaga = %d", pcb->estimacion_rafaga);
	log_info(logger, "************************************");
}

void print_traductor(t_log *logger, t_traductor *traductor) {
	log_info(logger, "************** TRADUCTOR ************");
	log_info(logger, "Cantidad entradas tabla = %d", traductor->cantidad_entradas_tabla);
	log_info(logger, "Tamanio pagina = %d", traductor->tamanio_pagina);
	log_info(logger, "************************************");
}
