#include "memoria.h"
#include "utils/hello.h"
#include <stdio.h>
#include <stdlib.h>

int PUERTO_MEMORIA = 0;

t_log* inicializar_log(void) {
  t_log* memoria_log = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO); // Crea .log
  if (memoria_log == NULL) {
    perror("ERROR - NO HAY LOGGER"); //Mensaje de error
    exit(EXIT_FAILURE);  // corta el programa
  }

  return memoria_log;
}

t_config* inicializar_config(void) {
  t_config* memoria_config = config_create("memoria.config");

  if (memoria_config == NULL) {
    perror("ERROR - NO HAY CONFIG");
    exit(EXIT_FAILURE);  // corta el programa
  }
  
  PUERTO_MEMORIA = config_get_int_value(memoria_config, "PUERTO_MEMORIA");
  TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
  TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
  ENTRADAS_POR_TABLA = config_get_int_value(memoria_config, "ENTRADAS_POR_TABLA");
  CANTIDAD_NIVELES = config_get_int_value(memoria_config, "CANTIDAD_NIVELES");
  RETARDO_MEMORIA = config_get_int_value(memoria_config, "RETARDO_MEMORIA");
  PATH_SWAPFILE = config_get_string_value(memoria_config, "PATH_SWAPFILE");
  RETARDO_SWAP = config_get_int_value(memoria_config, "RETARDO_SWAP");
  LOG_LEVEL = config_get_string_value(memoria_config, "LOG_LEVEL");
  DUMP_PATH = config_get_string_value(memoria_config, "DUMP_PATH");
  PATH_INSTRUCCIONES = config_get_string_value(memoria_config, "PATH_INSTRUCCIONES");

  return memoria_config;
}

argumentos_servidor_memoria* cargar_argumentos(t_log* logger, int puerto){
  argumentos_servidor_memoria* argumentos = malloc(sizeof(argumentos_servidor_memoria));
  argumentos->logger = logger;
  argumentos->puerto = puerto;

  return argumentos;
}

void* atender_cliente(void* arg){
  argumentos_servidor_memoria* data = (argumentos_servidor_memoria*) arg;
  int socket_server_memoria = iniciar_servidor(data->logger, data->puerto);
  if (socket_server_memoria == -1) {
    log_error(data->logger, "Error: fallo al iniciar servidor en puerto %d.", data->puerto);
    return NULL;
  }

  log_info(data->logger, "Servidor iniciado correctamente en puerto %d, esperando conexiones...", data->puerto);

  while (1) {
    struct sockaddr_in cliente_addr;
    socklen_t addr_len = sizeof(cliente_addr);
    int cliente_socket = accept(socket_server_memoria, (struct sockaddr*)&cliente_addr, &addr_len);
    if (cliente_socket == -1) {
      log_error(data->logger, "Error en accept()");
      continue;
    }

    log_info(data->logger, "Nueva conexión aceptada");

    // Aquí podés crear un hilo para atender la conexión o atenderla directamente.
    // Ejemplo directo (sin hilos):

    // atender_peticion(cliente_socket, data->logger);

    // Luego cerrás la conexión
    close(cliente_socket);
  }

  close(socket_server_memoria);
  return NULL;
}

void inicializar_estructuras (void){
  
  espacio_usuario = malloc(TAM_MEMORIA);
  if(espacio_usuario == NULL){
    log_error(logger, "Error al reservar el espacio de usuario.");
    exit (EXIT_FAILURE);
  }
  memset(espacio_usuario, 0, TAM_MEMORIA);
// RESERVA ESPACIO DE USUARIO
  int cant_marcos = TAM_MEMORIA / TAM_PAGINA;
  char* bits = malloc(cant_marcos / 8 + 1);
  bitmap_marcos = bitarray_create_with_mode (bits, cant_marcos/8 + 1, LSB_FIRST);
  for (int i = 0; i < cant_marcos; i++){
    bitarray_clean_bit(bitmap_marcos, i);
  } 
  log_info(logger, "Bitmap inicializado con %d marcos libres", cant_marcos);
// CREA LOS MARCOS Y LOS LIBERA TODOS AL INICIALIZAR

/* Como manejar: 
  bitarray_test_bit () Ver si un marco esta libre
  bitarray_set_bit () Ocupa un marco
  bitarray_clear_bit () Libera un marco
  bitarray_destroy () Libera el bitarray y los marcos internos
 */

  lista_procesos = list_create();
  pthread_mutex_init(&acc_lista_proc, NULL);

} 

int calculo_pags_necesarias (int tamanio){
  return (tamanio + TAM_PAGINA - 1)/ TAM_PAGINA;
} //Calcula cantidad de paginas necesarias

bool marcos_libres (int paginas){
  int libres = 0;
  int total_marcos = TAM_MEMORIA / TAM_PAGINA;
  for(int i = 0; i < total_marcos; i++){
    if(!bitarray_test_bit(bitmap_marcos, i)){
      libres++;
    }
  }
  return libres >= paginas;
} //Verifica espacio libre en memoria (Marcos)

void crear_proceso(int pid, int pags_necesarias){
  proceso_mem* proceso = malloc(sizeof(proceso_mem));
  proceso->pid = pid;
  proceso->paginas_asign = pags_necesarias;
  proceso->marcos_asign = malloc(sizeof(int)* pags_necesarias);

  int j = 0;
  for (int i = 0; j < pags_necesarias; i++){
    if(!bitarray_test_bit(bitmap_marcos, i)){
      bitarray_set_bit(bitmap_marcos, i);
      proceso->marcos_asign [j++] = i;

    }
  }

  proceso->instrucciones = cargar_instrucciones(pid);
  pthread_mutex_lock (&acc_lista_proc); //Aisla el proceso para agregarlo a la lista en memoria
  list_add(lista_procesos, proceso);
  pthread_mutext_unlock (&acc_lista_proc);

} //Crea la estructura del proceso

char** cargar_instrucciones(int pid){
  char path[256];
  sprintf(path, "%s%d", PATH_INSTRUCCIONES, pid);
  FILE* archivo = fopen(path, "r");
  if(!archivo){
    log_error(logger, "No se pudo abrir el script PID %d", pid);
    return NULL;
  } 
  
  char** instrucciones = NULL; //Array de strings
  char* linea = NULL; //Linea de archivo
  size_t len = 0;
  int cont = 0; //Cant de instrucciones leidas
  while (getline(&linea, &len, archivo) != -1){
    instrucciones = realloc(instrucciones, sizeof(char*) * (cont + 1));
    instrucciones [cont++] = strdup(strok(linea, "\n"));
  }

  fclose(archivo);
  free(linea);
  return instrucciones;

}

void manejo_init_proc (int socket_cliente){
  int pid;
  int tamanio;

  recv(socket_cliente, &pid, sizeof(int), 0);
  recv(socket_cliente, &tamanio, sizeof(int), 0);
  int paginas = calcular_paginas_necesarias(tamanio);
  if(hay_marcos_libres(paginas)){ //Crea si hay espacio
    crear_proceso(pid, paginas);
    int ok = 1;
    send (socket_cliente, &ok, sizeof(int), 0);
    log_info(logger, "## PID: %d - Proceso creado - Tamanio: %d", pid, tamanio);
  } else { //Si no hay espacio, tira error al logger
    int fail = 0;
    send (socket_cliente, &fail,sizeof(int), 0);
    log_info(logger, "## PID: %d - FALLO AL CREAR PROCESO - Memoria insuficiente", pid);
  }

} //Al recibir ordenes de KERNEL, crea un nuevo proceso SI HAY ESPACIO LIBRE
/*
1. ¿Cuántas páginas?
2. ¿Hay espacio libre?
  - NO → FAIL
  - SI → reservar marcos
      cargar script
      guardar en lista
      OK
*/