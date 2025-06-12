#include "utils/hello.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

int PUERTO_KERNEL = 0;
char* IP_KERNEL;
char* LOG_LEVEL;

t_log* inicializar_io_log() {
  t_log* cpu_log = log_create("io.log", "IO", true, LOG_LEVEL_INFO); //Crea el .log
  if (cpu_log == NULL) {
    perror("ERROR - NO HAY LOGGER"); //Mensaje de error
    exit(EXIT_FAILURE);
  }


  return cpu_log;
}

t_config* inicializar_io_config(void) {
  t_config* io_config = config_create("io.config");

  if (io_config == NULL) {
    perror("ERROR - NO HAY CONFIG");
    exit(EXIT_FAILURE);  // corta el programa
  }

  IP_KERNEL = config_get_string_value(io_config, "IP_KERNEL");
  PUERTO_KERNEL = config_get_int_value(io_config, "PUERTO_KERNEL");

  return io_config;
}