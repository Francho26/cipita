#include "utils/hello.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

char* IP_MEMORIA;
int PUERTO_MEMORIA = 0;
int PUERTO_ESCUCHA_DISPATCH = 0;
int PUERTO_ESCUCHA_INTERRUPT = 0;
int PUERTO_ESCUCHA_IO = 0;
char* ALGORITMO_CORTO_PLAZO;
char* ALGORITMO_INGRESO_A_READY;
int ALFA = 0;
int ESTIMACION_INICIA = 0;
int TIEMPO_SUSPENSION = 0;
char* LOG_LEVEL;


t_log* inicializar_kernel_log(void){
  t_log* kernel_log = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
  if (kernel_log == NULL) {
    perror("ERROR - NO HAY LOGGER"); //Mensaje de error
    exit(EXIT_FAILURE);
  }


  return kernel_log;
}

t_config* inicializar_kernel_config(void) {
  t_config* kernel_config = config_create("kernel.config");

  if (kernel_config == NULL) {
    perror("ERROR - NO HAY CONFIG");
    exit(EXIT_FAILURE);  // corta el programa
  }

  PUERTO_ESCUCHA_IO = config_get_int_value(kernel_config, "PUERTO_ESCUCHA_IO");
  IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
  PUERTO_MEMORIA = config_get_int_value(kernel_config, "PUERTO_MEMORIA");
  PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(kernel_config, "PUERTO_ESCUCHA_INTERRUPT");
  PUERTO_ESCUCHA_DISPATCH = config_get_int_value(kernel_config, "PUERTO_ESCUCHA_DISPATCH");
  //ip_CPU = config_get_string_value(config, "IP_CPU"); // Recibe IO

  return kernel_config;
}