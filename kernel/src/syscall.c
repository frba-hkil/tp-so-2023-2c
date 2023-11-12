#include "syscall.h"

void atender_cpu(t_pcb* pcb, t_protocolo protocolo) {

	switch(protocolo){
	case DESALOJO_POR_SYSCALL:
		atender_syscall(pcb);
		break;
	default:
		++(pcb->contexto->program_counter);
	}
}

void atender_syscall(t_pcb* pcb) {
	t_instruccion* inst = (t_instruccion *) list_get(pcb->contexto->instrucciones, pcb->contexto->program_counter);

	if(inst->identificador == SLEEP)
		bloquear_por_sleep(pcb);
}


void bloquear_por_sleep(t_pcb *pcb) {
	pthread_t thread_sleep;

	pthread_create(&thread_sleep, NULL, (void*)bloquearse_por, pcb);
	pthread_detach(thread_sleep);
}

void bloquearse_por(void* pcb) {
	t_pcb *_pcb = (t_pcb*) pcb;
	t_instruccion inst = *(t_instruccion *) list_get(_pcb->contexto->instrucciones, _pcb->contexto->program_counter);
	int segundos = atoi(inst.primer_operando);

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
		if(!queue_is_empty(cola_blocked_sleep)) {
			queue_push(cola_blocked_sleep, _pcb);
			_pcb = queue_pop(cola_blocked_sleep);
		}
		pthread_mutex_unlock(&mutex_cola_sleep);

		_pcb->estado = READY;
		++(_pcb->contexto->program_counter);

		pthread_mutex_lock(&mutex_lista_ready);
		list_add(lista_ready, _pcb);
		pthread_mutex_unlock(&mutex_lista_ready);
		sem_post(&sem_lista_ready);
	}


}
