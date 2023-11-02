#include "kernel_operaciones.h"

char *op_strings[] = {"NO_OP", "SET", "SUM", "SUB", "JNZ", "SLEEP", "WAIT", "SIGNAL",
                                "MOV_IN", "MOV_OUT", "F_OPEN", "F_CLOSE", "F_SEEK", "F_WRITE",
                                "F_TRUNCATE", "EXIT"};


void escuchar_consola(void) {

	generador_de_id = 0;
	plani_running = 0;
	command_handlers[0] = iniciar_proceso;
	command_handlers[2] = detener_planificacion;
	command_handlers[3] = iniciar_planificacion;
	command_handlers[4] = cambiar_multiprogramacion;
	cola_new = queue_create();
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_plani_running, NULL);
	sem_init(&sem_planificacion_l, 0, 0);
	sem_init(&sem_planificacion_c, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, kernel_config->grado_multiprogramacion);
	sem_init(&sem_new, 0, 0);
	//sem_init(&sem_exit, 0, 0);

	//sleep(1);
	pthread_create(&plani_largo_thread, NULL, (void*) plani_largo_pl, NULL);
	pthread_create(&plani_corto_thread, NULL, (void*) plani_corto_pl, kernel_config->algoritmo_planificacion);

	while(1){
		sem_wait(&sem_consola);

		//pthread_mutex_lock(&mutex_inst_consola);
		command_handlers[codigo_consola](parametros_consola);
		//pthread_mutex_unlock(&mutex_inst_consola);

	}
}

void iniciar_proceso(char** parametros) {
	FILE* proceso = fopen(parametros[0], "r");
	t_pcb* new_pcb;
	if(!proceso){
		log_error(kernel_logger, "ruta invalida / no existe (%s).\n", parametros[0]);
	}
	else{
		//nadie mas que el hilo donde crea procesos escribe sobre el pid, no hay por que usar un mutex
		new_pcb = crear_pcb(generador_de_id++, atoi(parametros[1]), crear_instrucciones(proceso), 0, atoi(parametros[2]));
		//print_pcb(kernel_logger, new_pcb);
		//print_instrucciones(kernel_logger, new_pcb->contexto->instrucciones);
		pthread_mutex_lock(&mutex_new);
		queue_push(cola_new, new_pcb);
		pthread_mutex_unlock(&mutex_new);
		sem_post(&sem_new);
		log_info(kernel_logger, "Se crea el proceso <%d> en NEW", new_pcb->contexto->pid);
	}
	fclose(proceso);
	//eliminar_pcb(new_pcb);
}

void finalizar_proceso(char** parametros) {
	//si esta en running(en cpu), mando una interrupcion para terminar
	//espero (hago un sem_wait) hasta que la cpu me devuelta el contexto
	//si esta en cualquier otro estado lo termino en el instante
	//funcion: liberar recursos y cambiar estado a EXIT
	//signal(grado multiprogramacion)
}

void iniciar_planificacion(char** parametros) {

	if(!plani_running) {
		pthread_mutex_lock(&mutex_plani_running);
		plani_running = 1;
		pthread_mutex_unlock(&mutex_plani_running);
		sem_post(&sem_planificacion_l);
		sem_post(&sem_planificacion_c);
		pthread_cond_signal(&cond_plani_running);
	}

}

void detener_planificacion(char** parametros) {

	if(plani_running) {
		pthread_mutex_lock(&mutex_plani_running);
		plani_running = 0;
		pthread_mutex_unlock(&mutex_plani_running);
	}
}

void cambiar_multiprogramacion(char** parametros) {
	sem_destroy(&sem_grado_multiprogramacion);
	sem_init(&sem_grado_multiprogramacion, 0, atoi(parametros[0]));
}


t_list *crear_instrucciones(FILE* proc) {

	char *op, *param1, *param2;
	uint32_t op_code;
	t_instruccion *inst;
	t_list *instrucciones = list_create();

	while(!feof(proc)) {
		op = string_new();
		param1 = string_new();
		param2 = string_new();
		fscanf(proc, "%s %s %s\n", op, param1, param2); //no verifica errores sintacticos/semanticos
		op_code = string_a_op_code(op);
		inst = crear_instruccion(op_code, param1, param2);
		list_add(instrucciones, inst);
		free(op);
		free(param1);
		free(param2);
	}

	return instrucciones;
}

t_op_code string_a_op_code(char* str) {
	int op_codes_size = string_array_size(op_strings);
	t_op_code i;

	for(i = 0; i < op_codes_size; i++) {
		if(string_equals_ignore_case(str, op_strings[i]))
			return i;
	}
	return -1;
}

