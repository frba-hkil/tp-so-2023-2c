#include "consola.h"

void iniciar_consola(void){
	pthread_t hilo;

	pthread_create(&hilo, NULL, &console_handler, NULL);
}

void console_handler(void){
	char* string;
	while(1){
		string = readline(">>");

		if(string_equals_ignore_case(string, "")){

		}
	}
}
