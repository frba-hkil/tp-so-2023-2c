#include "planificadores.h"

void plani_largo_pl(void) {

	char hilos_creados = 0;

	pthread_cond_init(&cond_plani_running, NULL);
	pthread_cond_init(&cond_ready_agregado, NULL);
	pthread_mutex_init(&mutex_lista_ready, NULL);
	sem_init(&sem_lista_ready, 0, 0);
	lista_ready = list_create();
	proceso_en_exec = NULL;

	while(1) {

		//pthread_mutex_lock(&mutex_plani_running);
		if(plani_running) {
			//pthread_mutex_unlock(&mutex_plani_running);

			if(!hilos_creados) {
				pthread_create(&thread_admitir, NULL, (void*) admitir_procesos, NULL);
				pthread_create(&thread_finalizar, NULL, (void*) finalizar_procesos, NULL);
				hilos_creados = 1;
			}
			//pthread_mutex_unlock(&mutex_plani_running);

		}

		else {
			//printf("\npausando L...\n");
			//pthread_mutex_unlock(&mutex_plani_running);
			sem_wait(&sem_planificacion_l);
		}

	}
}

void plani_corto_pl(char* algoritmo) {
	void (*planificador)(t_list*);
	cola_blocked_sleep = queue_create();
	pthread_t thread_atender_blocked;
	sem_init(&sem_check_deadlock, 0, 0);
	sem_init(&retomar, 0, 0);

	if(string_equals_ignore_case(algoritmo, "prioridades"))
		planificador = &prioridades;

	else if (string_equals_ignore_case(algoritmo, "round robin")) {
		planificador = &round_robin;
	}

	else {
		planificador = &fifo;
	}

	pthread_create(&thread_atender_blocked, NULL, (void*)atender_deadlock , NULL);
	pthread_detach(thread_atender_blocked);
	while(1) {

		pthread_mutex_lock(&mutex_plani_running);
		if(plani_running) {
			pthread_mutex_unlock(&mutex_plani_running);
			//printf("\ncorriendo plani corto plazo...\n");
			//sleep(10);
			sleep(1);
			sem_wait(&sem_lista_ready); // si no hay mas procesos en ready se pausa
			//pthread_mutex_lock(&mutex_lista_ready);

			planificador(lista_ready);
		}

		else {
			//printf("\npausando C...\n");
			pthread_mutex_unlock(&mutex_plani_running);
			sem_wait(&sem_planificacion_c);
		}
	}
}

void admitir_procesos(void) {

	pthread_mutex_init(&mutex_lista_ready, NULL);

	while(1) {
		sem_wait(&sem_grado_multiprogramacion);
		sem_wait(&sem_new);
		//pthread_mutex_lock(&mutex_plani_running);
		if (!plani_running) {
			//pthread_mutex_unlock(&mutex_plani_running);


		    pthread_cond_wait(&cond_plani_running, &mutex_plani_running);
		    pthread_mutex_unlock(&mutex_plani_running);
		}

		else {
			//pthread_mutex_unlock(&mutex_plani_running);
		}
		//sem_wait(&sem_grado_multiprogramacion);
		//sem_wait(&sem_new);
		pthread_mutex_lock(&mutex_new);
		if(!queue_is_empty(cola_new)) {
			t_pcb *pcb_ready = queue_pop(cola_new);
			pthread_mutex_unlock(&mutex_new);

			pcb_new_a_ready(pcb_ready);
			pthread_cond_signal(&cond_ready_agregado);
		}
		else
			pthread_mutex_unlock(&mutex_new);
	}
}


void finalizar_procesos(void) {
	t_pcb *pcb_exit;

	while(1) {

		sem_wait(&sem_exit);
		pthread_mutex_lock(&mutex_exit);
		pcb_exit = queue_pop(cola_exit);
		pthread_mutex_unlock(&mutex_exit);

		pcb_a_exit(pcb_exit);
		sem_post(&sem_grado_multiprogramacion);
	}
}

void fin_proc(uint32_t _pid) {
	t_pcb* pcb = NULL;

	pcb = buscar_proceso_cola(cola_new, _pid);

	if(!pcb) {
		pcb = buscar_proceso_lista(lista_ready, _pid);
	}
	if(!pcb) {
		pcb = buscar_proceso_diccionario(colas_blocked, _pid);
	}
	if(pcb) {
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		sem_post(&sem_exit);
		return;
	}
	if(proceso_en_exec == pcb) {
		enviar_mensaje("finalizar", DESALOJO_POR_EXIT, sockets[SOCK_CPU_INT]);
	}
	if(!pcb) {
		log_info(kernel_logger, "FINALIZAR_PROCESO: no existe el proceso con PID <%d>", _pid);
	}
}


void fifo(t_list* procesos_en_ready) {
	t_pcb* pcb;

	pthread_mutex_lock(&mutex_lista_ready);
	pcb = list_remove(procesos_en_ready, 0);
	pthread_mutex_unlock(&mutex_lista_ready);

	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);
	pcb->estado = EXEC;
	/*
	pthread_mutex_lock(&mutex_proceso_en_exec);
	proceso_en_exec = pcb;
	pthread_mutex_unlock(&mutex_proceso_en_exec);
	*/
	pthread_mutex_lock(&mutex_bloqueado);
	bloqueado = false;
	pthread_mutex_unlock(&mutex_bloqueado);
	do {
		t_paquete* paquete = serializar_contexto_ejecucion(pcb->contexto, CONTEXTO_EJECUCION);
		enviar_paquete(paquete, sockets[SOCK_CPU_DISPATCH]);
		eliminar_paquete(paquete);
		paquete = recibir_paquete(sockets[SOCK_CPU_DISPATCH]);

		eliminar_contexto_ejecucion(pcb->contexto);
		pcb->contexto = malloc(sizeof(t_contexto_ejecucion));

		deserializar_contexto_ejecucion(pcb->contexto, paquete);

		if(pcb->contexto->inst_desalojador->identificador != EXIT) {
			atender_cpu(pcb, paquete->codigo_operacion);

		}
		else {
			pthread_mutex_lock(&mutex_exit);
			queue_push(cola_exit, pcb);
			pthread_mutex_unlock(&mutex_exit);
			sem_post(&sem_exit);
		}

		eliminar_paquete(paquete);
	} while(!bloqueado && pcb->contexto->inst_desalojador->identificador != EXIT);
}

void atender_deadlock(void) {
	t_list *keys_colas = dictionary_keys(recursos_sistema);
	int cant_recursos = dictionary_size(recursos_sistema), i = 0;
	t_list *posibles_procesos;

	while(1) {

		sem_wait(&sem_check_deadlock);

		posibles_procesos = list_create();
		bool estado_seguro = true;
		char* recurso_actual;

		char* recurso_inicial = (char*)list_get(keys_colas, 0);
		t_queue *cola_blocked_recurso = (t_queue*)dictionary_get(colas_blocked, recurso_inicial);
		while(estado_seguro && i < cant_recursos) {

			if(queue_is_empty(cola_blocked_recurso))
				break;
			else {
				int j = 0;
				t_pcb *pcb = (t_pcb*)queue_peek(cola_blocked_recurso);
				while(j < cant_recursos) {
					recurso_actual = list_get(keys_colas, j);
					uint32_t *instancia_asignado = (uint32_t*)dictionary_get(pcb->recursos_asignados, recurso_actual);
					if(instancia_asignado) {

						if(string_equals_ignore_case(recurso_actual, recurso_inicial)) {
							estado_seguro = false;
						}
						else
							break;
					}
					j++;
				}
				list_add(posibles_procesos, pcb);
			}
			i++;
			cola_blocked_recurso = (t_queue*)dictionary_get(colas_blocked, recurso_actual);
		}

		if(!estado_seguro) {
			mostrar_procesos_involucrados_deadlock(posibles_procesos);
		}

		list_destroy(posibles_procesos);
		i = 0;
	}
	//sem_post(&retomar);
}

void mostrar_procesos_involucrados_deadlock(t_list *procesos) {
	int cant_procesos = list_size(procesos), i, cant_recursos, j;
	t_pcb* proceso;
	char* recursos_asignados;
	t_list* keys;

	for(i = 0; i < cant_procesos; i++) {
		proceso = (t_pcb*)list_get(procesos, i);
		cant_recursos = dictionary_size(proceso->recursos_asignados);
		keys = dictionary_keys(proceso->recursos_asignados);

		recursos_asignados = string_duplicate(list_get(keys, 0));

		for(j = 1 ; j < cant_recursos; j++){
			string_append_with_format(&recursos_asignados, " %s", (char*)list_get(keys, j));
		}
		log_info(kernel_logger, "Deadlock detectado: <%d> - Recursos en posesión <%s> - Recurso requerido: <%s>", proceso->contexto->pid, recursos_asignados, proceso->recurso_pendiente);
		free(recursos_asignados);
		//list_clean_and_destroy_elements(keys, free);
	}
}

t_pcb* buscar_proceso_cola(t_queue *cola, int pid) {
	int i = 0, cant_elementos = queue_size(cola);
	t_pcb* proceso;

	while(i < cant_elementos) {

		proceso = list_get(cola->elements, i);

		if(proceso->contexto->pid == pid) {
			t_pcb * ret = list_remove(cola->elements, i);
			return ret;
		}
		i++;
	}

	return NULL;
}

t_pcb* buscar_proceso_lista(t_list *lista, int pid) {
	int i = 0, cant_elementos = list_size(lista);
	t_pcb* proceso;

	while(i < cant_elementos) {

		proceso = list_get(lista, i);

		if(proceso->contexto->pid == pid) {
			t_pcb *ret = list_remove(lista, i);
			return ret;
		}
		i++;
	}

	return NULL;
}

t_pcb* buscar_proceso_diccionario(t_dictionary *dict, int pid) {
	int i = 0, cant_elementos = dictionary_size(dict);
	t_list *keys = dictionary_keys(dict);
	t_pcb* proceso;

	while(i < cant_elementos) {
		char* key = list_get(keys, i);
		t_queue *cola_block = (t_queue*)dictionary_get(dict, key);
		proceso = buscar_proceso_cola(cola_block, pid);

		if(proceso) {
			//list_clean_and_destroy_elements(keys, free);
			return proceso;
		}
		i++;
	}

	return NULL;
}

