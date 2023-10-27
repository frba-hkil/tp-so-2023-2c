#include "consola.h"

char* comandos[CANT_MAX_COMANDOS] = {"iniciar_proceso", "finalizar_proceso", "detener_planificacion", "iniciar_planificacion", "multiprogramacion", "proceso_estado"};

extern pthread_t console_thread;

void iniciar_consola(void){

	pthread_create(&console_thread, NULL, console_routine, (void*)console_handler);

}

void* console_routine(void* arg){
	void (*consola)(void) = (void (*)(void))arg;
	consola();

	return NULL;
}

char** console_handler(void){
	char* input;
	char** parametros;

	sem_init(&sem_consola, 0, 0);
	pthread_mutex_init(&mutex_inst_consola, NULL);

	while(1){
		parametros = string_array_new();
		input = readline(">>");
		if(input)
			add_history(input);

		char **s = string_n_split(input, 2," ");

		if(s[1] != NULL){
			parametros = string_n_split(s[1], 3, " ");
			string_iterate_lines(parametros, &print_param);
		}
		else{
			printf("parametro vacio\n");
		}

		interpretar_comando(s[0], parametros);

		free(input);
		string_array_destroy(s);
		string_array_destroy(parametros);
	}

}

void interpretar_comando(char* input, char** parametros){
	Comando i = 0;

	while(i < 6){
		if(string_equals_ignore_case(input, comandos[i])){
			//(*command_handlers[i])(parametros);
			log_info(kernel_logger, "comando encontrado: %s", comandos[i]);

			//pthread_mutex_lock(&mutex_inst_consola);
			codigo_consola = i;
			parametros_consola = copy_string_array(parametros, string_array_size(parametros));
			//pthread_mutex_unlock(&mutex_inst_consola);
			sem_post(&sem_consola);
			//printf("\ncomando encontrado: %s\n", comandos[i]);
			break;
		}
		i++;
	}
	if(i >= 6){
		log_error(kernel_logger, "comando no existe");
		//printf("\ncomando no existe\n");
	}
}

void print_param(char* param){
	printf("%s ", param);
}

char** copy_string_array(char** original, int size) {
    // Allocate memory for the new array of strings
    char** copy = (char**)malloc(size * sizeof(char*));

    if (copy != NULL) {
        for (int i = 0; i < size; ++i) {

            copy[i] = strdup(original[i]);

            if (copy[i] == NULL) {

                for (int j = 0; j < i; ++j) {
                    free(copy[j]);
                }
                free(copy);
                return NULL;
            }
        }
    }

    return copy;
}
