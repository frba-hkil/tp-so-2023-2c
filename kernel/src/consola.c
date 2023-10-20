#include "consola.h"

char* comandos[CANT_MAX_COMANDOS] = {"iniciar_proceso", "finalizar_proceso", "detener_planificacion", "iniciar_planificacion", "multiprogramacion", "proceso_estado"};

void iniciar_consola(void){
	pthread_t hilo;

	pthread_create(&hilo, NULL, console_routine, (void*)console_handler);
	pthread_join(hilo, NULL);
}

void* console_routine(void* arg){
	void (*consola)(void) = (void (*)(void))arg;
	consola();

	return NULL;
}

void console_handler(void){
	char* input;
	char** parametros;

	while(1){
		parametros = string_array_new();
		input = readline(">>");

		char **s = string_n_split(input, 2," ");

		if(s[1] != NULL){
			parametros = string_n_split(s[1], 3, " ");
			string_iterate_lines(parametros, &printf);
		}
		else{
			printf("parametro vacio\n");
		}
		ejecutar_comando(s[0], parametros);
		free(input);
		string_array_destroy(s);
		string_array_destroy(parametros);
	}

}

void ejecutar_comando(char* input, char** parametros){
	Comando i = 0;

	while(i < 6){
		if(string_equals_ignore_case(input, comandos[i])){
			//(*command_handlers[i])(parametros);
			printf("\ninstruccion encontrado: %s\n", comandos[i]);
			break;
		}
		i++;
	}
	if(i >= 6)
		printf("\ninstruccion no existe\n");
}

int iniciar_proceso(char** parametros){

}

