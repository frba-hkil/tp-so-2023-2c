#include "planificadores.h"

void plani_largo_pl(void) {

	char hilos_creados = 0;

	pthread_cond_init(&cond_plani_running, NULL);
	pthread_cond_init(&cond_ready_agregado, NULL);
	pthread_mutex_init(&mutex_lista_ready, NULL);
	sem_init(&sem_lista_ready, 0, 0);
	lista_ready = list_create();

	while(1) {

		//pthread_mutex_lock(&mutex_plani_running);
		if(plani_running) {
			//pthread_mutex_unlock(&mutex_plani_running);

			if(!hilos_creados) {
				pthread_create(&thread_admitir, NULL, (void*) admitir_procesos, NULL);
				//pthread_create(&thread_finalizar, NULL, (void*) finalizar_procesos, NULL);
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

	if(string_equals_ignore_case(algoritmo, "prioridades"))
		planificador = &prioridades;

	else if (string_equals_ignore_case(algoritmo, "round robin")) {
		planificador = &round_robin;
	}

	else {
		planificador = &fifo;
	}

	pthread_create(&thread_atender_blocked, NULL, (void*)atender_bloqueados , NULL);

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
			//pthread_mutex_unlock(&mutex_lista_ready);
			//log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb_a_ejecutar->contexto->pid);
			//sem_post(&sem_grado_multiprogramacion);
			//mandar a cpu y esperar respuesta(contexto)
			//actualizar pcb. si devolvio contexto por EXIT agregar a cola de exit
			//sem_post(&sem_exit); si es por EXIT
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

		//pthread_mutex_lock(&mutex_plani_running);
		if (!plani_running) {
			//pthread_mutex_unlock(&mutex_plani_running);


		    pthread_cond_wait(&cond_plani_running, &mutex_plani_running);
		    pthread_mutex_unlock(&mutex_plani_running);
		}

		else {
			//pthread_mutex_unlock(&mutex_plani_running);
		}
		sem_wait(&sem_grado_multiprogramacion);
		sem_wait(&sem_new);
		pthread_mutex_lock(&mutex_new);
		t_pcb *pcb_ready = queue_pop(cola_new);
		pthread_mutex_unlock(&mutex_new);

		pcb_new_a_ready(pcb_ready);
		pthread_cond_signal(&cond_ready_agregado);
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


void fifo(t_list* procesos_en_ready) {
	t_pcb* pcb;

	pthread_mutex_lock(&mutex_lista_ready);
	pcb = list_remove(procesos_en_ready, 0);
	pthread_mutex_unlock(&mutex_lista_ready);

	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);

	t_paquete* paquete = serializar_contexto_ejecucion(pcb->contexto, PCB);
	enviar_paquete(paquete, sockets[SOCK_CPU_DISPATCH]);
	eliminar_paquete(paquete);
	paquete = recibir_paquete(sockets[SOCK_CPU_DISPATCH]);

	int op_code = *(int*)list_get(pcb->contexto->instrucciones, pcb->contexto->program_counter);

	if(op_code != EXIT) {
		pthread_mutex_lock(&mutex_lista_ready);
		list_add(procesos_en_ready, pcb); //por ahora no pasa a estado bloqueado
		pthread_mutex_unlock(&mutex_lista_ready);
	}
	else {
		pthread_mutex_lock(&mutex_exit);
		queue_push(cola_exit, pcb);
		pthread_mutex_unlock(&mutex_exit);
		sem_post(&sem_exit);
		//mandarlo a cola de exit. (signal a hilo de finalizar proceso de planificador largo)
		//hacer un signal de grado de multiprogramacion
	}
}

void atender_bloqueados(void) {

}









