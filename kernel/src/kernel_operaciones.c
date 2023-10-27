#include "kernel_operaciones.h"

char *op_strings[] = {"NO_OP", "SET", "SUM", "SUB", "JNZ", "SLEEP", "WAIT", "SIGNAL",
                                "MOV_IN", "MOV_OUT", "F_OPEN", "F_CLOSE", "F_SEEK", "F_WRITE",
                                "F_TRUNCATE", "EXIT"};

void escuchar_consola(void){

	pthread_mutex_init(&mutex_generador_id, NULL);
	generador_de_id = 0;
	command_handlers[0] = iniciar_proceso;

	while(1){
		sem_wait(&sem_consola);
		printf("recibi comando\n");
		command_handlers[codigo_consola](parametros_consola);

	}
}

void iniciar_proceso(char** parametros){
	FILE* proceso = fopen(parametros[0], "r");
	t_pcb* new_pcb;
	if(!proceso){
		log_error(kernel_logger, "ruta invalida / no existe (%s).\n", parametros[0]);
	}
	else{
		new_pcb = crear_pcb(crear_pid(), atoi(parametros[1]), crear_instrucciones(proceso), 0, atoi(parametros[2]));
		print_pcb(kernel_logger, new_pcb);
		print_instrucciones(kernel_logger, new_pcb->contexto->instrucciones);
	}
	fclose(proceso);
	eliminar_pcb(new_pcb);
}

uint32_t crear_pid(void){

	pthread_mutex_lock(&mutex_generador_id);
	uint32_t id = generador_de_id++;
	pthread_mutex_unlock(&mutex_generador_id);

	return id;
}

t_list *crear_instrucciones(FILE* proc) {

	char *op;
	uint32_t op_code, param1, param2;
	t_instruccion *inst;
	t_list *instrucciones = list_create();

	while(!feof(proc)) {
		op = string_new();
		fscanf(proc, "%s %d %d\n", op, &param1, &param2); //no verifica errores sintacticos/semanticos
		op_code = string_a_op_code(op);
		inst = crear_instruccion(op_code, param1, param2);
		list_add(instrucciones, inst);
		free(op);
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

