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

	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);
	pcb->estado = EXEC;

	pthread_mutex_lock(&mutex_proceso_en_exec);
	proceso_en_exec = pcb;
	pthread_mutex_unlock(&mutex_proceso_en_exec);

	pthread_mutex_lock(&mutex_bloqueado);
	bloqueado = false;
	pthread_mutex_unlock(&mutex_bloqueado);

	do {
		t_paquete* packet = serializar_contexto_ejecucion(pcb->contexto, CONTEXTO_EJECUCION);
		enviar_paquete(packet, sockets[SOCK_CPU_DISPATCH]);
		eliminar_paquete(packet);

		pthread_create(&thread_check, NULL, (void*)check_higher_prio, pcb);
		pthread_detach(thread_check);
		packet = recibir_paquete(sockets[SOCK_CPU_DISPATCH]);
		contexto_devuelto = true;

		eliminar_contexto_ejecucion(pcb->contexto);
		pcb->contexto = malloc(sizeof(t_contexto_ejecucion));

		deserializar_contexto_ejecucion(pcb->contexto, packet);

		if(pcb->contexto->inst_desalojador->identificador != EXIT) {
			atender_cpu(pcb, packet->codigo_operacion);
		}
		else {
			pthread_mutex_lock(&mutex_exit);
			queue_push(cola_exit, pcb);
			pthread_mutex_unlock(&mutex_exit);
			sem_post(&sem_exit);
		}

		eliminar_paquete(packet);
	} while(!bloqueado && pcb->contexto->inst_desalojador->identificador != EXIT);
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
		if(pcb != proceso_en_exec) {
			enviar_mensaje("mayor_prio", DESALOJAR_PROCESO, sockets[SOCK_CPU_INT]);
		}
	}
}

