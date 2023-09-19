#include "main.h"


int main(int argc, char *argv[]) {
    t_config *cfg = config_create("./cpu.config");
    if (cfg == NULL) {
        error_show("El archivo de configuracion no existe");
        return EXIT_FAILURE;
    }

    // TEST 

    //t_pcb *testPcb = crear_pcb(1, 1, list_create(), 1, 1);

    // char *code = string_new();
    // string_append(&code, "SET AX 11\n");
    // string_append(&code, "SET BX 50\n");
    // string_append(&code, "SUM AX BX\n");
    // string_append(&code, "SET CX 5\n");
    // string_append(&code, "SUB BX CX\n");
    // string_append(&code, "EXIT");

    // procesarInstrucciones(code, testPcb);    

    // printf("AX %d\n", testPcb->AX);
    // printf("BX %d\n", testPcb->BX);
    // printf("CX %d\n", testPcb->CX);
    // printf("DX %d\n", testPcb->DX);

    config_destroy(cfg);
    return 0;
}

void procesarInstrucciones(char *codigo, t_pcb *pcb) {
    char **rows = string_split(codigo, "\n"), **cells;
    int i = 0;

    while (rows[i] != NULL) {
        cells = string_split(rows[i], " ");
        char *instruccion = cells[0];
        char **args = NULL;

        if (cells[1] != NULL) {
            args = string_array_new();
            int k = 1;

            while (cells[k] != NULL) {
                string_array_push(&args, cells[k]);
                k++;
            }
        }

        ejecutarInstruccion(instruccion, args, pcb);

        i++;
    } 
}

void set_registro(t_pcb *pcb, char *reg, u_int32_t val) {
    if (reg[0] == 'A') 
        pcb->AX = val;
    else if(reg[0] == 'B') 
        pcb->BX = val;
    else if(reg[0] == 'C') 
        pcb->CX = val;
    else if(reg[0] == 'D') 
        pcb->DX = val;
}

 
uint32_t get_registro(t_pcb *pcb, char *reg) {
    if (reg[0] == 'A') 
        return pcb->AX;
    else if(reg[0] == 'B') 
        return pcb->BX;
    else if(reg[0] == 'C') 
        return pcb->CX;
    else if(reg[0] == 'D') 
        return pcb->DX;

    return 0;
}

uint32_t transform_value(t_pcb *pcb, char *val) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(nombres_registros[i], val) == 0) {
            return get_registro(pcb, val);
        }
    }

    return atoi(val);
}

void ejecutarInstruccion(char *tipo, char **args, t_pcb *pcb) {
    if (strcmp(tipo, "SET") == 0) {
        set_registro(pcb, args[0], transform_value(pcb, args[1]));
    } else if (strcmp(tipo, "SUM") == 0) {
        set_registro(pcb, args[0], get_registro(pcb, args[0]) + get_registro(pcb, args[1]));
    } else if (strcmp(tipo, "SUB") == 0) {
        set_registro(pcb, args[0], get_registro(pcb, args[0]) - get_registro(pcb, args[1]));
    }
}

