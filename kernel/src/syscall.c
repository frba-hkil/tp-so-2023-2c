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

	int32_t *cantidad_recurso;

	pthread_mutex_lock(&mutex_recursos_s);
	string_trim_right(&pcb->contexto->inst_desalojador->primer_operando);

	if(dictionary_has_key(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando)) {
		cantidad_recurso = (int32_t*)dictionary_get(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando);
		if(*cantidad_recurso - 1 >= 0) {
			--(*cantidad_recurso);
			pthread_mutex_unlock(&mutex_recursos_s);
			pthread_mutex_lock(&mutex_recursos_p);

			if(!dictionary_has_key(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando)) {
				int32_t *instancia = malloc(sizeof(int32_t));
				*instancia = 1;
				dictionary_put(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando, instancia);
			}
			else {
				int32_t *instancia = dictionary_get(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando);
				++(*instancia);
			}
			pthread_mutex_unlock(&mutex_recursos_p);
		}
		else {
			pcb->recurso_pendiente = string_duplicate(pcb->contexto->inst_desalojador->primer_operando);
			pthread_mutex_unlock(&mutex_recursos_s);
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
		sem_post(&sem_check_deadlock);
		//sem_wait(&retomar);
	}
	else {
		pthread_mutex_unlock(&mutex_recursos_s);
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		bloqueado = true;
		sem_post(&sem_exit);
	}

}

void signal(t_pcb* pcb) {
	string_trim_right(&pcb->contexto->inst_desalojador->primer_operando);
	if(dictionary_has_key(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando)) {
		pthread_mutex_lock(&mutex_recursos_p);

		if(dictionary_has_key(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando)) {
			int32_t *recurso_proceso = (int32_t*)dictionary_get(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando);
			--(*recurso_proceso);
			if(*recurso_proceso == 0) {
				dictionary_remove_and_destroy(pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando, free);
			}
			pthread_mutex_unlock(&mutex_recursos_p);
			pthread_mutex_lock(&mutex_cola_bloqueado);
			t_queue* cola_blocked = (t_queue*) dictionary_get(colas_blocked, pcb->contexto->inst_desalojador->primer_operando);
			if(!queue_is_empty(cola_blocked)) {
				t_pcb * unlocked_pcb = queue_pop(cola_blocked);
				pthread_mutex_unlock(&mutex_cola_bloqueado);
				free(unlocked_pcb->recurso_pendiente);
				unlocked_pcb->recurso_pendiente = string_new();
				int32_t *instancia_nueva = malloc(sizeof(int32_t));
				*instancia_nueva = 1;
				dictionary_put(unlocked_pcb->recursos_asignados, pcb->contexto->inst_desalojador->primer_operando, instancia_nueva);
				unlocked_pcb->estado = READY;
				pthread_mutex_lock(&mutex_lista_ready);
				list_add(lista_ready, unlocked_pcb);
				pthread_mutex_unlock(&mutex_lista_ready);
				sem_post(&sem_lista_ready);
			}
			else {
				pthread_mutex_unlock(&mutex_cola_bloqueado);
				pthread_mutex_lock(&mutex_recursos_s);
				int32_t *cantidad_recurso = (int32_t*)dictionary_get(recursos_sistema, pcb->contexto->inst_desalojador->primer_operando);
				++(*cantidad_recurso);
				pthread_mutex_unlock(&mutex_recursos_s);
			}
		}
		else {
			pthread_mutex_lock(&mutex_exit);
			queue_push(cola_exit, pcb);
			pthread_mutex_unlock(&mutex_exit);
			bloqueado = true;
			sem_post(&sem_exit);
		}
	}
	else {
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		bloqueado = true;
		sem_post(&sem_exit);
	}
}

void liberar_recursos_asignados(t_pcb* pcb) {
	if(!dictionary_is_empty(pcb->recursos_asignados))
		dictionary_iterator(pcb->recursos_asignados, liberar_recurso);
}

void liberar_recurso(char* _key, void* n) {
	int32_t *instancia = (int32_t*) n, *m;

	t_queue* cola_blocked = (t_queue*) dictionary_get(colas_blocked, _key);

	m = (int32_t*)dictionary_get(recursos_sistema, _key);

	int i;
	for(i = 0; i < *instancia; i++){
		if(!queue_is_empty(cola_blocked)) {
			pthread_mutex_lock(&mutex_cola_bloqueado);
			t_pcb * unlocked_pcb = queue_pop(cola_blocked);
			unlocked_pcb->estado = READY;
			pthread_mutex_unlock(&mutex_cola_bloqueado);
			pthread_mutex_lock(&mutex_recursos_p);
			//int32_t *n_actual_proceso = (int32_t*) dictionary_get(unlocked_pcb->recursos_asignados, _key);
			//++(*n_actual_proceso);
			int32_t *nueva_instancia = malloc(sizeof(int32_t));
			*nueva_instancia = 1;
			dictionary_put(unlocked_pcb->recursos_asignados, _key, nueva_instancia);
			pthread_mutex_unlock(&mutex_recursos_p);
			pthread_mutex_lock(&mutex_lista_ready);
			list_add(lista_ready, unlocked_pcb);
			pthread_mutex_unlock(&mutex_lista_ready);
			sem_post(&sem_lista_ready);
		}
		else {
			pthread_mutex_lock(&mutex_recursos_s);
			++(*m);
			pthread_mutex_unlock(&mutex_recursos_s);
		}
	}
	*instancia = 0;
}
