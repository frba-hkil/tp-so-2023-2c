#include "round_robin.h"

bool contexto_devuelto;
pthread_t thread_quantum;

void round_robin(t_list* procesos_en_ready) {
	t_pcb* pcb;
	contexto_devuelto = false;

	pthread_mutex_lock(&mutex_lista_ready);
	pcb = list_remove(procesos_en_ready, 0);
	pthread_mutex_unlock(&mutex_lista_ready);

	log_info(kernel_logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", pcb->contexto->pid);
	t_paquete* packet = serializar_contexto_ejecucion(pcb->contexto, CONTEXTO_EJECUCION);
	enviar_paquete(packet, sockets[SOCK_CPU_DISPATCH]);
	eliminar_paquete(packet);
	pthread_create(&thread_quantum, NULL, (void*)contar_quantum, NULL);
	pthread_detach(thread_quantum);
	packet = recibir_paquete(sockets[SOCK_CPU_DISPATCH]);
	contexto_devuelto = true;

	eliminar_contexto_ejecucion(pcb->contexto);
	pcb->contexto = malloc(sizeof(t_contexto_ejecucion));

	deserializar_contexto_ejecucion(pcb->contexto, packet);

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

	//eliminar_paquete(packet);

}

void contar_quantum(void) {
	int timer = kernel_config->quantum / 1000; //solo funcionaria correctamente si recibe segundos enteros

	sleep(timer);
	if(!contexto_devuelto) {
		log_info(kernel_logger, "termino quantum");
		//enviar_mensaje("fin_quantum", DESALOJAR_PROCESO, sockets[SOCK_INT]);
	}

}
