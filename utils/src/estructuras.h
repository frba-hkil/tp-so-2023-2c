#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>

typedef enum {
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
	F_READ,
	F_WRITE,
	F_TRUNCATE,
	EXIT
} t_op_code;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCKED,
	_EXIT
} t_estado;

typedef struct {
	t_op_code identificador;
	char* primer_operando;
	char* segundo_operando;
} t_instruccion;

typedef struct {
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
} t_registro;

typedef struct {
	uint32_t pid;
	t_instruccion *inst_desalojador; //instruccion que hizo desalojar si corresponde
	uint32_t program_counter;
	t_registro *registros;

} t_contexto_ejecucion;

typedef struct {
	t_contexto_ejecucion *contexto;
	uint32_t tamanio_proceso;
	//uint32_t tabla_paginas;
	uint32_t prioridad;
	t_estado estado;
	t_dictionary *recursos_asignados; //key: nombre recurso, elemento: cantidad de recurso
	char *recurso_pendiente;// lista de t_recurso
} t_pcb;

typedef struct {
	char* nombre;
	uint32_t cantidad;
} t_recurso;

typedef struct {
	int32_t nro_pag;
	int32_t desplazamiento;
} t_direccion_logica;

typedef struct {
	int32_t nro_frame;
	int32_t desplazamiento;
} t_direccion_fisica;

typedef struct {
	uint32_t nro_pagina; //nro pagina
	int64_t ultimo_acceso;
	uint32_t nro_frame;
	bool presente;
	bool modificado;
	uint32_t posicion_en_swap;
} t_entrada_tabla_pagina;

typedef struct {
	uint32_t pid;
	t_list *entradas; //lista de t_pagina
} t_tabla_pagina;

typedef struct {
	uint32_t cantidad_entradas_tabla;
	uint32_t tamanio_pagina;
} t_traductor;

typedef struct {
	uint32_t direccion;
	uint32_t entrada;
} t_tabla_acceso;

t_instruccion *crear_instruccion(t_op_code identificador, char* primer_operando, char* segundo_operando);
void eliminar_instrucciones(void *instrucciones);
void eliminar_contexto_ejecucion(t_contexto_ejecucion* ce);
t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, uint32_t prioridad);
void eliminar_pcb(t_pcb *pcb);
void eliminar_recursos_pendientes(void* _recurso);
t_traductor *crear_traductor_direcciones(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor_direcciones(t_traductor *traductor);
t_tabla_acceso *crear_tabla_de_acceso(uint32_t direccion, uint32_t entrada);
void eliminar_tabla_de_acceso(t_tabla_acceso *tabla_acceso);
void eliminar_instruccion(t_instruccion* inst);

#endif /* ESTRUCTURAS_H_ */

