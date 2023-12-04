#include "syscall.h"

void atender_cpu(t_pcb* pcb, t_protocolo protocolo) {

	switch(protocolo){
	case DESALOJO_POR_SYSCALL:
		atender_syscall(pcb);
		break;
	default:
		;
	}
}

void atender_syscall(t_pcb* pcb) {

	if(pcb->contexto->inst_desalojador->identificador == SLEEP) {
		bloqueado = true;
		bloquear_por_sleep(pcb);
	}
	else if(pcb->contexto->inst_desalojador->identificador == WAIT) {
		wait(pcb);
	}
	else if(pcb->contexto->inst_desalojador->identificador == SIGNAL) {
		signal(pcb);
	}
}


void bloquear_por_sleep(t_pcb *pcb) {
	pthread_t thread_sleep;

	pthread_create(&thread_sleep, NULL, (void*)bloquearse_por, pcb);
	pthread_detach(thread_sleep);
}

void bloquearse_por(void* pcb) { // bloquearse por un tiempo determinado.
	t_pcb *_pcb = (t_pcb*) pcb;

	int segundos = atoi(_pcb->contexto->inst_desalojador->primer_operando);
	_pcb->estado = BLOCKED;

	sleep(segundos);
	log_info(kernel_logger, "PID %d durmio---", _pcb->contexto->pid);

	if(!plani_running) {
		pthread_mutex_lock(&mutex_cola_sleep);
		queue_push(cola_blocked_sleep, pcb);
		pthread_mutex_unlock(&mutex_cola_sleep);
	}
	else {
		pthread_mutex_lock(&mutex_cola_sleep);
		while(!queue_is_empty(cola_blocked_sleep) && plani_running) {
			queue_push(cola_blocked_sleep, _pcb);
			_pcb = queue_pop(cola_blocked_sleep);
		}
		pthread_mutex_unlock(&mutex_cola_sleep);

		_pcb->estado = READY;

		pthread_mutex_lock(&mutex_lista_ready);
		list_add(lista_ready, _pcb);
		pthread_mutex_unlock(&mutex_lista_ready);
		sem_post(&sem_lista_ready);
	}


}

void wait(t_pcb* pcb) { //optamos por no crear un hilo aparte para controlar procesos bloqueados en este tp, ya que es un sistema monoprocesador
						//las funciones wait y signal controlaran las transiciones de exec->blocked y blocked->ready

	uint32_t *cantidad_recurso;

	if(dictionary_has_key(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando)) {
		cantidad_recurso = (uint32_t*)dictionary_get(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando);
		--(*cantidad_recurso);

		if((*cantidad_recurso) < 0) {
			pthread_mutex_lock(&mutex_bloqueado);
			bloqueado = true;
			pthread_mutex_unlock(&mutex_bloqueado);
			pthread_mutex_lock(&mutex_cola_bloqueado);
			t_queue* cola_blocked = (t_queue*) dictionary_get(colas_blocked, pcb->contexto->inst_desalojador->primer_operando);
			pcb->estado = BLOCKED;
			queue_push(cola_blocked, pcb);
			pthread_mutex_unlock(&mutex_cola_bloqueado);
			log_info(kernel_logger, "PID: %d - EXEC A BLOCKED", pcb->contexto->pid);
		}

		else {
			uint32_t *recurso_proceso = (uint32_t*)dictionary_get(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando);
			++(*recurso_proceso);
		}
	}
	else {
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		sem_post(&sem_exit);
	}

}

void signal(t_pcb* pcb) {

	if(dictionary_has_key(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando)) {
		uint32_t *recurso_proceso = (uint32_t*)dictionary_get(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando);

		if((*recurso_proceso) > 0) {
			--(*recurso_proceso);
			uint32_t *cantidad_recurso = (uint32_t*)dictionary_get(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando);
			++(*cantidad_recurso);
			t_queue* cola_blocked = (t_queue*) dictionary_get(colas_blocked, pcb->contexto->inst_desalojador->primer_operando);
			if(!queue_is_empty(cola_blocked)) {
				t_pcb * unlocked_pcb = queue_pop(cola_blocked);

				unlocked_pcb->estado = READY;
				pthread_mutex_lock(&mutex_lista_ready);
				list_add(lista_ready, unlocked_pcb);
				pthread_mutex_unlock(&mutex_lista_ready);
				sem_post(&sem_lista_ready);
			}
		}
		else {
			pthread_mutex_lock(&mutex_exit);
			queue_push(cola_exit, pcb);
			pthread_mutex_unlock(&mutex_exit);
			sem_post(&sem_exit);
		}
	}
	else {
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		sem_post(&sem_exit);
	}
}

