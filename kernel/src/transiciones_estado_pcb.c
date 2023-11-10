#include "transiciones_estado_pcb.h"

void pcb_new_a_ready(t_pcb* _pcb) {

	//uint32_t n = 1;

	_pcb->estado = READY;
	//handshake por ahora. reemplazarlo por la funcion que manda la solicitud para crear estructuras necesarias
	//send(sockets[SOCK_MEM],&n, sizeof(uint32_t), 0);
	//log_info(kernel_logger, "Cola Ready <%s>: [<LISTA DE PIDS>]", kernel_config->algoritmo_planificacion);
	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", _pcb->contexto->pid);

	pthread_mutex_lock(&mutex_lista_ready);
	list_add(lista_ready, _pcb);
	pthread_mutex_unlock(&mutex_lista_ready);
	sem_post(&sem_lista_ready); // avisa que hay procesos disponibles en ready para el planificador de corto plazo

}

void pcb_a_exit(t_pcb *_pcb) {
	char *estado = estado_a_string(_pcb->contexto->pid);

	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <EXIT>", estado, _pcb->contexto->pid);

	_pcb->estado = EXIT;
	//liberar_recursos(_pcb);

	eliminar_pcb(_pcb);
	free(estado);

}

char* estado_a_string(int estado) {
	char *c_estado = malloc(8*sizeof(char));

	switch(estado){
	case NEW:
		strcpy(c_estado, "NEW");
		break;
	case READY:
		strcpy(c_estado, "READY");
		break;
	case EXEC:
		strcpy(c_estado, "EXEC");
		break;
	case BLOCKED:
		strcpy(c_estado, "BLOCKED");
		break;
	default:
		strcpy(c_estado, "EXIT");
	}

	return c_estado;
}
