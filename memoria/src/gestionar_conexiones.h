#ifndef GESTIONAR_CONEXIONES_H_
#define GESTIONAR_CONEXIONES_H_

#include "memoria_global.h"
#include "cargar_instrucciones.h"
#include "mem_sockets.h"
#include "paginas.h"
#include <serializador.h>
#include <commons/string.h>
#include <estructuras.h>
#include <print.h>
//#include "memoria_principal.h"
//#include "paginacion.h"
//#include "swap.h"

pthread_t thread_kernel;
pthread_t thread_cpu;
pthread_t thread_fs;

//void procesar_conexiones(t_cliente *datos_cliente);
void enviar_direccion_tabla_primer_nivel(int socket_fd, uint32_t direccion);
void informar_estado_proceso(int socket_fd, t_protocolo protocolo);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
void enviar_direccion_tabla_segundo_nivel(int socket_fd, uint32_t direccion);
void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero);
void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor);
void informar_escritura_realizada(int socket_fd, t_protocolo protocolo);
void recibir_proceso(t_paquete* paquete);

void atender_kernel(void);
void atender_cpu(void);
void atender_fs(void);
void escuchar_kernel(void);
void escuchar_cpu(void);
void enviar_instruccion(t_contexto_ejecucion* ce, t_paquete* paquete);

#endif /* GESTIONAR_CONEXIONES_H_ */
