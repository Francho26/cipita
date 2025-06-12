#ifndef KERNEL_H
#define KERNEL_H

#include <commons/config.h>
#include <commons/log.h>

extern char* IP_MEMORIA;
extern int PUERTO_MEMORIA;
extern int PUERTO_ESCUCHA_DISPATCH;
extern int PUERTO_ESCUCHA_INTERRUPT;
extern int PUERTO_ESCUCHA_IO;
extern char* ALGORITMO_CORTO_PLAZO;
extern char* ALGORITMO_INGRESO_A_READY;
extern int ALFA;
extern int ESTIMACION_INICIAL;
extern int TIEMPO_SUSPENSION;
extern char* LOG_LEVEL;

// Funciones para inicializar log y configuración
t_log* inicializar_kernel_log(void);
t_config* inicializar_kernel_config(void);

#endif