#include "planificadores.h"

void plani_largo_pl(void) {

	char hilos_creados = 0;
	sem_init(&sem_seguir_admitiendo, 0, 0);
	pthread_cond_init(&cond_plani_running, NULL);
	//sem_init(&sem_seguir_finalizando, 0, 0);

	while(1) {

		pthread_mutex_lock(&mutex_plani_running);
		if(plani_running) {

			if(!hilos_creados) {
				pthread_create(&thread_admitir, NULL, (void*) admitir_procesos, NULL);
				//pthread_create(&thread_finalizar, NULL, (void*) finalizar_procesos, NULL);
				hilos_creados = 1;
			}
			pthread_mutex_unlock(&mutex_plani_running);
			//printf("\ncorriendo plani largo plazo...\n");
			//sleep(10);
			//sem_post(&sem_seguir_admitiendo);
			//sem_post(&sem_seguir_finalizando);
			sleep(1);
		}

		else {
			//printf("\npausando...\n");
			pthread_mutex_unlock(&mutex_plani_running);
			sem_wait(&sem_planificacion_l);
		}

	}
}

void plani_corto_pl(void) {

	while(1) {

		//pthread_mutex_lock(&mutex_plani_running);
		if(plani_running) {
			//pthread_mutex_unlock(&mutex_plani_running);
			//printf("\ncorriendo plani corto plazo...\n");
			//sleep(10);
			//elegir proceso a correr de acuerdo al algoritmo
			//mandar a cpu y esperar respuesta(contexto)
			//actualizar pcb. si devolvio contexto por EXIT agregar a cola de exit
			//sem_post(&sem_exit); si es por EXIT
		}

		else {
			//printf("\npausando...\n");
			sem_wait(&sem_planificacion_c);
		}
	}
}

void admitir_procesos(void) {

	while(1) {

		sem_wait(&sem_grado_multiprogramacion);
		//printf("\ngrado\n");
		sem_wait(&sem_new);
		//printf("\ncola\n");
		pthread_mutex_lock(&mutex_plani_running);
		if (!plani_running) {
			pthread_mutex_unlock(&mutex_plani_running);


		    pthread_cond_wait(&cond_plani_running, &mutex_plani_running);
		    pthread_mutex_unlock(&mutex_plani_running);
		}

		else {
			pthread_mutex_unlock(&mutex_plani_running);
		}

		//printf("\nsigo admitiendo\n");
		pthread_mutex_lock(&mutex_new);
		t_pcb *pcb_ready = queue_pop(cola_new);
		pthread_mutex_unlock(&mutex_new);

		pcb_new_a_ready(pcb_ready);
	}
}
/*
void finalizar_procesos(void) {

	while(1) {

		sem_wait(&sem_exit);
		sem_wait(&sem_seguir_finalizando);
		pthread_mutex_lock(&mutex_exit);
		t_pcb *pcb_exit = queue_pop(cola_exit);
		pthread_mutex_unlock(&mutex_exit);

		pcb_a_exit(pcb_exit);
	}
}
*/













