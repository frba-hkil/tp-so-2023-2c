#include "main.h"


int main(int argc, char *argv[]) {
    t_config *cfg = config_create("./cpu.config");
    if (cfg == NULL) {
        error_show("El archivo de configuracion no existe");
        return EXIT_FAILURE;
    }

    sockets.dispatch= iniciar_servidor(config_get_string_value(cfg, "IP_CPU"), config_get_string_value(cfg, "PUERTO_ESCUCHA_DISPATCH"));
    sockets.interrupt = iniciar_servidor(config_get_string_value(cfg, "IP_CPU"), config_get_string_value(cfg, "PUERTO_ESCUCHA_INTERRUPT"));
    sockets.memoria = crear_conexion(config_get_string_value(cfg, "IP_MEMORIA"), config_get_string_value(cfg, "PUERTO_MEMORIA"));

    iniciar_hilos_cpu();

    config_destroy(cfg);
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
    while (1) {
        t_paquete *pqt = recibir_paquete(sockets.dispatch);

        if (pqt != NULL) {
            t_contexto_ejecucion *cntx = malloc(sizeof(t_contexto_ejecucion));
            deserializar_contexto_ejecucion(cntx, pqt);
            ejecutarInstrucciones(cntx);
        }
    }
}

void hilo_interrupt() {

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
 
void ejecutarInstrucciones(t_contexto_ejecucion *contexto) {
    while (contexto->program_counter<=list_size(contexto->instrucciones)) {
        t_instruccion *instruccion = list_get(contexto->instrucciones, contexto->program_counter-1); //mem-leak?
        contexto->program_counter++;
        bool devolver;
        t_protocolo protocolo;

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
            devolver = true;
        } else if (instruccion->identificador == WAIT) {
            //WAIT (Recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro
            devolver = true;
        } else if (instruccion->identificador == SIGNAL) {
            //SIGNAL (Recurso): Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro.
            devolver = true;
        } else if (instruccion->identificador == MOV_IN) {
            //MOV_IN (Registro, Dirección Lógica): Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro.

        } else if (instruccion->identificador == MOV_OUT) {
            //MOV_OUT (Dirección Lógica, Registro): Lee el valor del Registro y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica.

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
        }

        if (devolver) {
            t_paquete *cntx = serializar_contexto_ejecucion(contexto, protocolo);
            enviar_paquete(cntx, sockets.dispatch);

            break;
        }
    }    
}