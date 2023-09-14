#include "main.h"


int main(int argc, char *argv[]) {
    t_config *cfg = config_create("./cpu.config");
    if (cfg == NULL) {
        error_show("El archivo de configuracion no existe");
        return EXIT_FAILURE;
    }

    // TEST 
    // char *code = string_new();
    // string_append(&code, "SET AX 11\n");
    // string_append(&code, "SET BX 23\n");
    // string_append(&code, "ADD AX BX\n");
    // string_append(&code, "EXIT");

    // procesarInstrucciones(code);

    config_destroy(cfg);
    return 0;
}

void procesarInstrucciones(char *codigo) {
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
        
        // TEST 
        // puts(instruccion);
        // if (args != NULL) {
        //     puts(args[0]);
        //     puts(args[1]);
        // }

        i++;
    } 
}