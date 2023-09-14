#include "main.h"


int main(int argc, char *argv[]) {
    t_config *cfg = config_create("./cpu.config");
    if (cfg == NULL) {
        error_show("El archivo de configuracion no existe");
        return EXIT_FAILURE;
    }


    config_destroy(cfg);
    return 0;
}
