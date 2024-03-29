#include "main.h"

int main(int argc, char *argv[]) {
    t_config *cfg = config_create("../cpu.config");
    if (cfg == NULL) {
        error_show("El archivo de configuracion no existe");
        return EXIT_FAILURE;
    }

    logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_DEBUG);

    sockets.dispatch= iniciar_modulo_servidor(config_get_string_value(cfg, "IP_CPU"), config_get_string_value(cfg, "PUERTO_ESCUCHA_DISPATCH"), logger);
    sockets.interrupt = iniciar_modulo_servidor(config_get_string_value(cfg, "IP_CPU"), config_get_string_value(cfg, "PUERTO_ESCUCHA_INTERRUPT"), logger);
    sockets.memoria = conectar_a_modulo(config_get_string_value(cfg, "IP_MEMORIA"), config_get_string_value(cfg, "PUERTO_MEMORIA"), logger);
    page_size = get_page_size();
    iniciar_hilos_cpu();

    config_destroy(cfg);
    log_destroy(logger);
    return 0;
}

void iniciar_hilos_cpu() {
    pthread_t t_hilo_dispatch, t_hilo_interrupt;

    pthread_create(&t_hilo_dispatch, NULL, (void *)hilo_dispatch, NULL);
    pthread_create(&t_hilo_interrupt, NULL, (void *)hilo_interrupt, NULL);

    pthread_join(t_hilo_dispatch, NULL);
    pthread_join(t_hilo_interrupt, NULL);
}

void hilo_dispatch() {
    t_contexto_ejecucion *cntx;

    int socket_cliente = esperar_cliente(sockets.dispatch);
    log_info(logger, "Cliente dispatch conectado");
    while (1) {
    	cntx = malloc(sizeof(t_contexto_ejecucion));
        t_paquete *pqt = recibir_paquete(socket_cliente);

        if (pqt->codigo_operacion == CONTEXTO_EJECUCION) {
            deserializar_contexto_ejecucion(cntx, pqt);

            log_debug(logger, "CNTX DUMP PC->%d, AX->%d, BX->%d, CX->%d, DX->%d", cntx->program_counter, cntx->registros->AX, cntx->registros->BX, cntx->registros->CX, cntx->registros->DX);
            ejecutarInstrucciones(cntx, socket_cliente);
            log_debug(logger, "CNTX DUMP PC->%d, AX->%d, BX->%d, CX->%d, DX->%d", cntx->program_counter, cntx->registros->AX, cntx->registros->BX, cntx->registros->CX, cntx->registros->DX);
        }
        eliminar_contexto_ejecucion(cntx);
        eliminar_paquete(pqt);
    }
}

void hilo_interrupt() {
    int socket_cliente = esperar_cliente(sockets.interrupt);
    log_info(logger, "Cliente interrupt conectado");
    while (1) { 
        t_paquete *pqt = recibir_paquete(socket_cliente);

        if (pqt->codigo_operacion==DESALOJAR_PROCESO) {
            desalojar = true;
        }
        else if (pqt->codigo_operacion == DESALOJO_POR_EXIT)
        	terminar = true;

        eliminar_paquete(pqt);
    }
}

void set_registro(t_contexto_ejecucion *contexto, char *reg, u_int32_t val) {
    if (reg[0] == 'A') 
        contexto->registros->AX = val;
    else if(reg[0] == 'B') 
        contexto->registros->BX = val;
    else if(reg[0] == 'C') 
        contexto->registros->CX = val;
    else if(reg[0] == 'D') 
        contexto->registros->DX = val;
}

uint32_t get_registro(t_contexto_ejecucion *contexto, char *reg) {
    if (reg[0] == 'A') 
        return contexto->registros->AX;
    else if(reg[0] == 'B') 
        return contexto->registros->BX;
    else if(reg[0] == 'C') 
        return contexto->registros->CX;
    else if(reg[0] == 'D') 
        return contexto->registros->DX;

    return 0;
}

uint32_t transform_value(t_contexto_ejecucion *contexto, char *val) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(nombres_registros[i], val) == 0) {
            return get_registro(contexto, val);
        }
    }

    return atoi(val);
}
 
char *getInstruccionLabel(t_op_code iId){
    if (iId == SET) return "SET";
    else if (iId == SUM) return "SUM";
    else if (iId == SUB) return "SUB";
    else if (iId == JNZ) return "JNZ";
    else if (iId == SLEEP) return "SLEEP";
    else if (iId == WAIT) return "WAIT";
    else if (iId == SIGNAL) return "SIGNAL";
    else if (iId == MOV_IN) return "MOV_IN";
    else if (iId == MOV_OUT) return "MOV_OUT";
    else if (iId == F_OPEN) return "F_OPEN";
    else if (iId == F_CLOSE) return "F_CLOSE";
    else if (iId == F_SEEK) return "F_SEEK";
    else if (iId == F_READ) return "F_READ";
    else if (iId == F_WRITE) return "F_WRITE";
    else if (iId == F_TRUNCATE) return "F_TRUNCATE";
    else if (iId == EXIT) return "EXIT";

    return "";
}

void ejecutarInstrucciones(t_contexto_ejecucion *contexto, int socket_kernel) {
    t_paquete *pqt;
    bool devolver;
    t_protocolo protocolo;

    while (true) {
        pqt = serializar_contexto_ejecucion(contexto, SOLICITAR_INSTRUCCION);

        enviar_paquete(pqt, sockets.memoria);
        eliminar_paquete(pqt);
        do {
            pqt = recibir_paquete(sockets.memoria);
            //log_info(logger, "recibi instruccion a ejecutar");
        } while (pqt->codigo_operacion != INSTRUCCION && pqt->codigo_operacion!=NO_INSTRUCCION);

        devolver = false;

        if (pqt->codigo_operacion == INSTRUCCION) {
            //++(contexto->program_counter);
            t_instruccion *instruccion = deserializar_instruccion(pqt);
            log_info(logger, "PID: %d - FETCH - Program Counter: %d", contexto->pid, contexto->program_counter);

            if (instruccion->identificador == SET) {
                set_registro(contexto, instruccion->primer_operando, transform_value(contexto, instruccion->segundo_operando));
            } else if (instruccion->identificador == SUM) {
                set_registro(contexto, instruccion->primer_operando, get_registro(contexto, instruccion->primer_operando) + get_registro(contexto, instruccion->segundo_operando));
            } else if (instruccion->identificador == SUB) {
                set_registro(contexto, instruccion->primer_operando, get_registro(contexto, instruccion->primer_operando) - get_registro(contexto, instruccion->segundo_operando));
            } else if (instruccion->identificador == JNZ) {
                if (get_registro(contexto, instruccion->primer_operando) != 0) {
                    contexto->program_counter = transform_value(contexto, instruccion->segundo_operando);
                }
            } else if (instruccion->identificador == SLEEP) {
                //SLEEP (Tiempo): Esta instrucción representa una syscall bloqueante. Se deberá devolver el Contexto de Ejecución actualizado al Kernel junto a la cantidad de segundos que va a bloquearse el proceso.
                protocolo = DESALOJO_POR_SYSCALL;
                devolver = true;
                contexto->inst_desalojador->identificador = instruccion->identificador;
                free(contexto->inst_desalojador->primer_operando);
                contexto->inst_desalojador->primer_operando = string_duplicate(instruccion->primer_operando);
                free(contexto->inst_desalojador->segundo_operando);
                contexto->inst_desalojador->segundo_operando = string_duplicate("\0");
            } else if (instruccion->identificador == WAIT) {
                //WAIT (Recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro
            	protocolo = DESALOJO_POR_SYSCALL;
                devolver = true;
                contexto->inst_desalojador->identificador = instruccion->identificador;
                free(contexto->inst_desalojador->primer_operando);
                contexto->inst_desalojador->primer_operando = string_duplicate(instruccion->primer_operando);
                free(contexto->inst_desalojador->segundo_operando);
                contexto->inst_desalojador->segundo_operando = string_duplicate("\0");
            } else if (instruccion->identificador == SIGNAL) {
                //SIGNAL (Recurso): Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro
            	protocolo = DESALOJO_POR_SYSCALL;
                devolver = true;
                contexto->inst_desalojador->identificador = instruccion->identificador;
                free(contexto->inst_desalojador->primer_operando);
                contexto->inst_desalojador->primer_operando = string_duplicate(instruccion->primer_operando);
                free(contexto->inst_desalojador->segundo_operando);
                contexto->inst_desalojador->segundo_operando = string_duplicate("\0");
            } else if (instruccion->identificador == MOV_IN) {
                //MOV_IN (Registro, Dirección Lógica): Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro.
                log_info(logger, "PID: %d - Accion: LEER - Direccion Fisica: <DIRECCION_FISICA> - Valor: <VALOR_LEIDO>", contexto->pid);
                nro_pagina = (atoi(instruccion->segundo_operando) / page_size);
                desplazamiento = atoi(instruccion->segundo_operando) - nro_pagina * page_size;
                uint32_t *nro_frame = solicitar_nro_frame(&nro_pagina, contexto->pid);
                if(!nro_frame) {
                	protocolo = DESALOJO_PAGE_FAULT;	//reconocemos que es una syscall mas, pero utilizamos un protocolo diferente para mas facilidad
                	devolver = true;
                	contexto->program_counter--;
                }
                else {
                	leer_de_memoria(contexto, instruccion->primer_operando, nro_frame, desplazamiento);
                }

            } else if (instruccion->identificador == MOV_OUT) {
                //MOV_OUT (Dirección Lógica, Registro): Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica.
                log_info(logger, "PID: %d - Accion: ESCRIBIR - Direccion Fisica: <DIRECCION_FISICA> - Valor: <VALOR_ESCRITO>", contexto->pid);
                nro_pagina = (atoi(instruccion->segundo_operando) / page_size);
                desplazamiento = atoi(instruccion->segundo_operando) - nro_pagina * page_size;
                uint32_t *nro_frame = solicitar_nro_frame(&nro_pagina, contexto->pid);
                if(!nro_frame) {
                protocolo = DESALOJO_PAGE_FAULT;	//reconocemos que es una syscall mas, pero utilizamos un protocolo diferente para mas facilidad
                devolver = true;
                contexto->program_counter--;
                }
                else {
                	escribir_en_memoria(contexto, instruccion->primer_operando, nro_frame, desplazamiento);
                }
            } else if (instruccion->identificador == F_OPEN) {
                //F_OPEN (Nombre Archivo, Modo Apertura): Esta instrucción solicita al kernel que abra el archivo pasado por parámetro con el modo de apertura indicado.
                devolver = true; 
                protocolo = DESALOJO_POR_IO;
            } else if (instruccion->identificador == F_CLOSE) {
                //F_CLOSE (Nombre Archivo): Esta instrucción solicita al kernel que cierre el archivo pasado por parámetro.
                devolver = true;
                protocolo = DESALOJO_POR_IO;
            } else if (instruccion->identificador == F_SEEK) {
                //F_SEEK (Nombre Archivo, Posición): Esta instrucción solicita al kernel actualizar el puntero del archivo a la posición pasada por parámetro.
                devolver = true;
                protocolo = DESALOJO_POR_IO;
            } else if (instruccion->identificador == F_READ) {
                //F_READ (Nombre Archivo, Dirección Lógica): Esta instrucción solicita al Kernel que se lea del archivo indicado y se escriba en la dirección física de Memoria la información leída.
                devolver = true;
                protocolo = DESALOJO_POR_IO;
            } else if (instruccion->identificador == F_WRITE) {
                //F_WRITE (Nombre Archivo, Dirección Lógica): Esta instrucción solicita al Kernel que se escriba en el archivo indicado la información que es obtenida a partir de la dirección física de Memoria.
                devolver = true;
                protocolo = DESALOJO_POR_IO;            
            } else if (instruccion->identificador == F_TRUNCATE) {
                //F_TRUNCATE (Nombre Archivo, Tamaño): Esta instrucción solicita al Kernel que se modifique el tamaño del archivo al indicado por parámetro.
                devolver = true;
                protocolo = DESALOJO_POR_IO;
            } else if (instruccion->identificador == EXIT) {
                //EXIT: Esta instrucción representa la syscall de finalización del proceso. Se deberá devolver el Contexto de Ejecución actualizado al Kernel para su finalización.
                devolver = true;
                protocolo = DESALOJO_POR_EXIT;
                contexto->inst_desalojador->identificador = instruccion->identificador;
                free(contexto->inst_desalojador->primer_operando);
                contexto->inst_desalojador->primer_operando = string_duplicate("\0");
                free(contexto->inst_desalojador->segundo_operando);
                contexto->inst_desalojador->segundo_operando = string_duplicate("\0");
            }

            log_info(logger, "PID: %d - Ejecutando: %s - %s %s", contexto->pid, getInstruccionLabel(instruccion->identificador), instruccion->primer_operando, instruccion->segundo_operando);
            free(instruccion);
        }

        contexto->program_counter++;

        if (desalojar) {
            devolver = true;
            protocolo = DESALOJO_POR_IRQ;
            desalojar = false;
        }
        if(terminar) {
        	devolver = true;
        	protocolo = DESALOJO_POR_EXIT;
        	contexto->inst_desalojador->identificador = EXIT;
        	free(contexto->inst_desalojador->primer_operando);
        	contexto->inst_desalojador->primer_operando = string_duplicate("\0");
        	free(contexto->inst_desalojador->segundo_operando);
        	contexto->inst_desalojador->segundo_operando = string_duplicate("\0");
        	terminar = false;
        }

        if (devolver) {
        	t_paquete *cntx = serializar_contexto_ejecucion(contexto, protocolo);
        	enviar_paquete(cntx, socket_kernel);
        	eliminar_paquete(cntx);
        	if(contexto->inst_desalojador->identificador == MOV_IN || contexto->inst_desalojador->identificador == MOV_OUT) {
        		cntx = crear_paquete(PAGE_FAULT, buffer_vacio());
        		agregar_a_paquete(cntx, &nro_pagina, sizeof(uint32_t));
        		enviar_paquete(cntx, socket_kernel);
        		eliminar_paquete(cntx);
        	}
            break;
        }

        eliminar_paquete(pqt);
    }    
}

uint32_t get_page_size(void) {
	t_paquete *paquete = recibir_paquete(sockets.memoria);
	t_list *data = deserealizar_paquete(paquete);
	uint32_t size = *(uint32_t*)list_get(data, 0);

	list_destroy_and_destroy_elements(data, free);
	eliminar_paquete(paquete);

	return size;
}

uint32_t *solicitar_nro_frame(uint32_t *n_pagina, uint32_t pid) {
	t_paquete *paquete = crear_paquete(ACCESO_TP, buffer_vacio());

	agregar_a_paquete(paquete, &pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, n_pagina, sizeof(uint32_t));
	enviar_paquete(paquete, sockets.memoria);
	eliminar_paquete(paquete);
	paquete = recibir_paquete(sockets.memoria);
	t_list *data = deserealizar_paquete(paquete);
	uint32_t *nro_frame = (uint32_t*)list_get(data, 0);

	free(list_get(data, 0));
	list_destroy(data);
	eliminar_paquete(paquete);

	return nro_frame;
}

void leer_de_memoria(t_contexto_ejecucion *contexto, char* reg, uint32_t *nro_frame, uint32_t desplazamiento) {
	t_paquete *paquete = crear_paquete(LEER_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &contexto->pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, nro_frame, sizeof(uint32_t));
	agregar_a_paquete(paquete, &desplazamiento, sizeof(uint32_t));

	enviar_paquete(paquete, sockets.memoria);
	eliminar_paquete(paquete);

	paquete = recibir_paquete(sockets.memoria);
	t_list* data = deserealizar_paquete(paquete);
	uint32_t valor_leido = *(uint32_t*)list_get(data, 0);

	if (reg[0] == 'A')
	    contexto->registros->AX = valor_leido;
	else if(reg[0] == 'B')
	    contexto->registros->BX = valor_leido;
	else if(reg[0] == 'C')
	    contexto->registros->CX = valor_leido;
	else if(reg[0] == 'D')
	    contexto->registros->DX = valor_leido;

	list_destroy_and_destroy_elements(data, free);
	eliminar_paquete(paquete);
}

void escribir_en_memoria(t_contexto_ejecucion *contexto, char* reg, uint32_t *nro_frame, uint32_t desplazamiento) {
	t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &contexto->pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, nro_frame, sizeof(uint32_t));
	agregar_a_paquete(paquete, &desplazamiento, sizeof(uint32_t));
	//paquete = recibir_paquete(sockets.memoria);
	uint32_t *valor_a_escribir;

	if (reg[0] == 'A')
	   valor_a_escribir = &contexto->registros->AX;
	else if(reg[0] == 'B')
		valor_a_escribir = &contexto->registros->BX;
	else if(reg[0] == 'C')
		valor_a_escribir = &contexto->registros->CX;
	else if(reg[0] == 'D')
		valor_a_escribir = &contexto->registros->DX;
	agregar_a_paquete(paquete, valor_a_escribir, sizeof(uint32_t));
	enviar_paquete(paquete, sockets.memoria);
	eliminar_paquete(paquete);
}
