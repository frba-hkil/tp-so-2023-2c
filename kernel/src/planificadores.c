#include "planificadores.h"

void plani_largo_pl(void) {

	while(1) {

		if(plani_running) {
			//hacer lo que tenga que hacer
			printf("\ncorriendo plani largo plazo...\n");
			sleep(5);
		}

		else {
			printf("\npausando...\n");
			sem_wait(&sem_planificacion_l);
		}
	}
}

void plani_corto_pl(void) {

	while(1) {

		if(plani_running) {
			printf("\ncorriendo plani corto plazo...\n");
			sleep(5);
			//hacer lo que tenga que hacer
		}

		else {
			printf("\npausando...\n");
			sem_wait(&sem_planificacion_c);
		}
	}
}
