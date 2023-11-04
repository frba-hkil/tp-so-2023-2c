#include "prioridad.h"

pthread_t thread_check;
//pthread_mutex_t mutex_contexto_devuelto = PTHREAD_MUTEX_INITIALIZER;
bool contexto_devuelto;

void prioridades(t_list* procesos_en_ready) {
	t_pcb *pcb;
	contexto_devuelto = false;

	if(list_size(lista_ready) >= 2) {
		pthread_mutex_lock(&mutex_lista_ready);
		pcb = list_get_minimum(lista_ready, mayor_prioridad);
		list_remove_element(lista_ready, pcb);
		pthread_mutex_unlock(&mutex_lista_ready);
	}

	else {
		pthread_mutex_lock(&mutex_lista_ready);
		pcb = list_remove(lista_ready, 0);
		pthread_mutex_unlock(&mutex_lista_ready);
	}

	t_paquete* packet = serializar_contexto_ejecucion(pcb->contexto, PCB);
	enviar_paquete(packet, sockets[0]); //reemplazar los constantes por SOCK_CPU_DISPATCH
	pthread_create(&thread_check, NULL, (void*)check_higher_prio, pcb);
	recibir_paquete(sockets[0]);
	contexto_devuelto = true;

	int op_code = *(int*)list_get(pcb->contexto->instrucciones, pcb->contexto->program_counter);

	if(op_code != EXIT) {
		pthread_mutex_lock(&mutex_lista_ready);
		list_add(procesos_en_ready, pcb); //por ahora no pasa a estado bloqueado
		pthread_mutex_unlock(&mutex_lista_ready);
	}
	else {
		//mandarlo a cola de exit. (signal a hilo de finalizar proceso de planificador largo)
		//hacer un signal de grado de multiprogramacion
	}


	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);
}

void* mayor_prioridad(void* pcb1, void* pcb2) {
	t_pcb*_pcb1 = (t_pcb*)pcb1;
	t_pcb*_pcb2 = (t_pcb*)pcb2;

	return (_pcb1->prioridad < _pcb2->prioridad) ? _pcb1 : _pcb2;
}

void check_higher_prio(t_pcb* proceso_en_exec) {
	t_pcb* pcb;

	while(!contexto_devuelto) {

		pthread_cond_wait(&cond_ready_agregado, &mutex_lista_ready);
		pcb = list_get_minimum(lista_ready, mayor_prioridad);
		pthread_mutex_unlock(&mutex_lista_ready);
		if(pcb == proceso_en_exec) {
			//enviar_mensaje("mayor_prio", DESALOJAR_PROCESO, sockets[SOCK_INT]);
		}
	}
}

