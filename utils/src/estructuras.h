#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>

typedef enum {
	NO_OP,
	SET,
	SUM,
	SUB,
	JNZ,
	SLEEP,
	WAIT,
	SIGNAL,
	MOV_IN,
	MOV_OUT,
	F_OPEN,
	F_CLOSE,
	F_SEEK,
	F_WRITE,
	F_TRUNCATE,
	EXIT
} t_op_code;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCKED,
	_EXIT,
	//SUSPENDED_BLOCKED,
	//SUSPENDED_READY
} t_estado;

typedef struct {
	t_op_code identificador;
	uint32_t primer_operando;
	uint32_t segundo_operando;
} t_instruccion;

typedef struct {
	t_list *instrucciones;
	uint32_t tamanio;
} t_consola;

typedef struct {
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
} t_registro;

typedef struct {
	uint32_t pid;
	t_list *instrucciones;
	uint32_t program_counter;
	t_registro *registros;

} t_contexto_ejecucion;

typedef struct {
	t_contexto_ejecucion *contexto;
	uint32_t tamanio_proceso;
	//uint32_t tabla_paginas;
	uint32_t prioridad;
} t_pcb;

typedef struct {
	uint32_t cantidad_entradas_tabla;
	uint32_t tamanio_pagina;
} t_traductor;

typedef struct {
	uint32_t direccion;
	uint32_t entrada;
} t_tabla_acceso;


t_instruccion *crear_instruccion(t_op_code identificador, uint32_t primer_operando, uint32_t segundo_operando);
void eliminar_instrucciones(t_list *instrucciones);
t_consola *crear_consola(t_list *instrucciones, uint32_t tamanio);
void eliminar_consola(t_consola *consola);
t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, t_list *instrucciones, uint32_t program_counter, uint32_t prioridad);
void eliminar_pcb(t_pcb *pcb);
t_traductor *crear_traductor_direcciones(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor_direcciones(t_traductor *traductor);
t_tabla_acceso *crear_tabla_de_acceso(uint32_t direccion, uint32_t entrada);
void eliminar_tabla_de_acceso(t_tabla_acceso *tabla_acceso);

#endif /* ESTRUCTURAS_H_ */

