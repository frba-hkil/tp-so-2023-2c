#include "serializador.h"

t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo);
t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos);

t_paquete *serializar_contexto_ejecucion(t_contexto_ejecucion *ce, t_protocolo protocolo) {
    t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());

    agregar_a_paquete(paquete, &(ce->pid), sizeof(uint32_t));
    agregar_a_paquete(paquete, ce->registros, sizeof(t_registro));  // Assuming t_registro is a fixed size
    agregar_a_paquete(paquete, &(ce->program_counter), sizeof(uint32_t));

    agregar_a_paquete(paquete, &(ce->inst_desalojador->identificador), sizeof(t_op_code));
    agregar_a_paquete(paquete, ce->inst_desalojador->primer_operando, strlen(ce->inst_desalojador->primer_operando)+1);
    agregar_a_paquete(paquete, ce->inst_desalojador->segundo_operando, strlen(ce->inst_desalojador->segundo_operando)+1);

    return paquete;
}

void deserializar_contexto_ejecucion(t_contexto_ejecucion *contexto, t_paquete *paquete) {
    t_list *datos = deserealizar_paquete(paquete);

    //contexto->instrucciones = deserializar_instrucciones(datos, list_size(datos) - 3);

    //uint32_t offset = list_size(contexto->instrucciones) * 3;
    contexto->pid = *(uint32_t *)list_get(datos, 0);
    contexto->registros = malloc(sizeof(t_registro));
    memcpy(contexto->registros, list_get(datos, 1), sizeof(t_registro));
    contexto->program_counter = *(uint32_t *)list_get(datos, 2);
    contexto->inst_desalojador = malloc(sizeof(t_instruccion));
    contexto->inst_desalojador->identificador = *(t_op_code*)list_get(datos, 3);
    contexto->inst_desalojador->primer_operando = string_duplicate((char*) list_get(datos, 4));
    contexto->inst_desalojador->segundo_operando = string_duplicate((char*) list_get(datos, 5));

    list_destroy_and_destroy_elements(datos, free);
}
/*
t_paquete *serializar_pcb(t_pcb *pcb, t_protocolo protocolo) {
    t_paquete *paquete = serializar_contexto_ejecucion(pcb->contexto, protocolo);

    agregar_a_paquete(paquete, &(pcb->tamanio_proceso), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->prioridad), sizeof(uint32_t));
    uint32_t estado = (uint32_t)pcb->estado;
    agregar_a_paquete(paquete, &estado, sizeof(uint32_t));

    return paquete;
}

t_pcb *deserializar_pcb(t_paquete *paquete) {
    t_list *datos = deserealizar_paquete(paquete);

    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->contexto = malloc(sizeof(t_contexto_ejecucion));
    pcb->contexto->registros = malloc(sizeof(t_registro));

    //deserializar_contexto_ejecucion(pcb->contexto, paquete);
    pcb->contexto->instrucciones = deserializar_instrucciones(datos, list_size(datos)-6);

    uint32_t offset = list_size(pcb->contexto->instrucciones) * 3;
    printf("\n%d %d\n",list_size(datos)-3, list_size(pcb->contexto->instrucciones) * 3);
    pcb->contexto->pid = *(uint32_t *)list_get(datos, offset + 0);
    memcpy(pcb->contexto->registros, list_get(datos, offset + 1), sizeof(t_registro));
    pcb->contexto->program_counter = *(uint32_t *)list_get(datos, offset + 2);
    pcb->tamanio_proceso = *(uint32_t *)list_get(datos, offset + 3);
    pcb->prioridad = *(uint32_t *)list_get(datos, offset + 4);
    pcb->estado = *(t_estado *)list_get(datos, offset + 5);


    list_destroy_and_destroy_elements(datos, free);
    return pcb;
}
*/
t_paquete *serializar_instruccion(t_instruccion *instruccion, t_protocolo protocolo) {
    t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
    agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_op_code));
    agregar_a_paquete(paquete, instruccion->primer_operando, strlen(instruccion->primer_operando)+1);
    agregar_a_paquete(paquete, instruccion->segundo_operando, strlen(instruccion->segundo_operando)+1);

    return paquete;
}

t_instruccion *deserializar_instruccion(t_paquete *paquete) {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    t_list *datos = deserealizar_paquete(paquete);

    instruccion->identificador = *(t_op_code *)list_get(datos, 0);
    instruccion->primer_operando = string_duplicate((char *)list_get(datos, 1));
    instruccion->segundo_operando = string_duplicate((char *)list_get(datos, 2));

    list_destroy_and_destroy_elements(datos, free);

    return instruccion;
}

t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());

	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_op_code));
		agregar_a_paquete(paquete, instruccion->primer_operando, strlen(instruccion->primer_operando)+1);
		agregar_a_paquete(paquete, instruccion->segundo_operando, strlen(instruccion->segundo_operando)+1);
	}

	return paquete;
}

t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos) {
	t_list *instrucciones = list_create();

	for(int i = 0; i < longitud_datos; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_op_code *)list_get(datos, i);
		instruccion->primer_operando = string_duplicate((char *)list_get(datos, i + 1));
		instruccion->segundo_operando = string_duplicate((char *)list_get(datos, i + 2));
		list_add(instrucciones, instruccion);
	}

	return instrucciones;
}

t_paquete *serializar_valores_enteros(t_list* valores, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	int i, cant_valores = list_size(valores);

	for(i = 0; i < cant_valores; i++) {
		uint32_t *valor = (uint32_t*) list_get(valores, i);
		agregar_a_paquete(paquete, valor, sizeof(uint32_t));
	}

	return paquete;
}

t_list *deserializar_valores_enteros(t_paquete *paquete) {
	t_list *valores = deserealizar_paquete(paquete);

	return valores;
}

