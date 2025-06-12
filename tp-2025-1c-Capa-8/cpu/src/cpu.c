#include "utils/hello.h"
#include "utils/protocolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include "cpu.h"

char* IP_MEMORIA;
int PUERTO_MEMORIA = 0;
char* IP_KERNEL;
int PUERTO_KERNEL_DISPATCH = 0;
int PUERTO_KERNEL_INTERRUPT = 0;
int ENTRADAS_TLB = 0;
char* REEMPLAZO_TLB;
int ENTRADAS_CACHE = 0;
char* REEMPLAZO_CACHE;
int RETARDO_CACHE = 0;
char* LOG_LEVEL;

t_log* inicializar_cpu_log(char* identificador_cpu){
  char log_identificador_cpu[64];
  snprintf(log_identificador_cpu, sizeof(log_identificador_cpu), "%s.log", identificador_cpu);
  t_log* cpu_log = log_create(log_identificador_cpu, identificador_cpu, true, LOG_LEVEL_INFO); //Crea el .log
  if (cpu_log == NULL) {
    perror("ERROR - NO HAY LOGGER"); //Mensaje de error
    exit(EXIT_FAILURE);
  }


  return cpu_log;
}

t_config* inicializar_cpu_config(void) {
  t_config* cpu_config = config_create("config/cpu.config");

  if (cpu_config == NULL) {
    perror("ERROR - NO HAY CONFIG");
    exit(EXIT_FAILURE);  // corta el programa
  }

  IP_KERNEL = config_get_string_value(cpu_config, "IP_KERNEL");
  IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
  PUERTO_MEMORIA = config_get_int_value(cpu_config, "PUERTO_MEMORIA");
  PUERTO_KERNEL_INTERRUPT = config_get_int_value(cpu_config, "PUERTO_KERNEL_INTERRUPT");
  PUERTO_KERNEL_DISPATCH = config_get_int_value(cpu_config, "PUERTO_KERNEL_DISPATCH");
  //ip_CPU = config_get_string_value(config, "IP_CPU"); // Recibe IO

  return cpu_config;
}

argumentos_conexion* cargar_argumentos(char* ip, t_log* logger, int  puerto, char* identificador) {
  argumentos_conexion* argumentos = malloc(sizeof(argumentos_conexion));
  argumentos->ip = ip;
  argumentos->logger = logger;
  argumentos->puerto = puerto;
  argumentos->identificador = identificador;
  return argumentos;
}

void* conectar_kernel_dispatch(void* args) {
  argumentos_conexion* data = (argumentos_conexion*)args;
  
  // Conexion al kernel
  int socket_dispatch = crear_conexion(data->logger, data->ip, data->puerto);
  if (socket_dispatch == -1) {
    log_error(data->logger, "Fallo la conexión con DISPATCH.");
    free(data);
    pthread_exit(NULL);
  }

  log_info(data->logger, "Conectado a DISPATCH en %s:%d", data->ip, data->puerto);
  log_info(data->logger, "%s conectada. Esperando peticiones...", data->identificador);

  // Enviar handshake
  char* handshake = data->identificador;
  uint32_t size = strlen(handshake) + 1;
  send(socket_dispatch, &size, sizeof(uint32_t), 0);
  send(socket_dispatch, handshake, size, 0);
  log_info(data->logger, "Handshake enviado con ID: %s", data->identificador);
  log_info(data->logger, "Conexión y handshake exitosos");

  // Recibir PCBs
  while (1) {
    log_info(data->logger, "Esperando PCB desde Kernel...");

    // Primero recibe el tamaño a leer
    uint32_t size_paquete;
    int recvd = recv(socket_dispatch, &size_paquete, sizeof(uint32_t), MSG_WAITALL);
    if (recvd <= 0) {
      log_error(data->logger, "Fallo al recibir tamaño del paquete del Kernel.");
      break;
    }

    // Reservo buffer para el paquete
    void* buffer_paquete = malloc(size_paquete);
    if (!buffer_paquete) {
      log_error(data->logger, "No se pudo reservar memoria para paquete.");
      break;
    }

    // Recibo el stream completo del paquete
    recvd = recv(socket_dispatch, buffer_paquete, size_paquete, MSG_WAITALL);
    if (recvd <= 0) {
      log_error(data->logger, "Fallo al recibir paquete completo del Kernel.");
      free(buffer_paquete);
      break;
    }

    // Reconstruir paquete
    t_buffer* tbuffer = malloc(sizeof(t_buffer));
    tbuffer->size = size_paquete;
    tbuffer->stream = buffer_paquete;

    t_paquete paquete;
    paquete.buffer = tbuffer;

    // Desempaqueto el pcb
    pcb_t* pcb = desempaquetar_pcb_t(&paquete);

    if (pcb == NULL) {
      log_error(data->logger, "Error al desempaquetar PCB.");
      free(tbuffer);
      break;
    }

    log_info(data->logger, "## PID: %d - FETCH - Program Counter: %d", pcb->pid, pcb->pc);

    // Conexión a Memoria (efímera)
    int socket_memoria = crear_conexion(data->logger, IP_MEMORIA, PUERTO_MEMORIA);
    if (socket_memoria == -1) {
      log_error(data->logger, "No se pudo conectar a Memoria para hacer FETCH");
      free(pcb);
      free(tbuffer);
      continue;
    }

    // Enviar el PC para obtener la instrucción
    send(socket_memoria, &(pcb->pc), sizeof(int), 0);

    // Recibir la instrucción
    char instruccion[256]; 
    recv(socket_memoria, instruccion, sizeof(instruccion), MSG_WAITALL);
    if (recvd <= 0) {
      log_error(data->logger, "Fallo al recibir instrucción de Memoria.");
      close(socket_memoria);
      free(pcb);
      continue;
    }

    // a modo debug
    log_info(data->logger, "Instrucción recibida (cruda): '%s'", instruccion);

    // Parseo instruccion
    instruccion_t_completa* instruccion_completa = parsear_instruccion(instruccion);
    if (!instruccion_completa) {
      log_error(data->logger, "Error al parsear instrucción.");
      close(socket_memoria);
      free(pcb);
      continue;
    }
    
    log_info(data->logger, "Instrucción recibida: %s", instruccion_to_string(instruccion_completa->tipo));
    switch (instruccion_completa->tipo) {
    case NOOP:
    log_info(data->logger, "Ejecutando NOOP");
    // NOOP no hace nada más
    break;

    case WRITE:
    log_info(data->logger, "Ejecutando WRITE: var=%s, valor=%s",
      instruccion_completa->args[0], instruccion_completa->args[1]);
    // Aquí podés poner la lógica de WRITE
    break;

    case READ:
    log_info(data->logger, "Ejecutando READ: var=%s", instruccion_completa->args[0]);
    // Aquí podés poner la lógica de READ
    break;

    case GOTO:
    log_info(data->logger, "Ejecutando GOTO a la posición %d",
      atoi(instruccion_completa->args[0]));
    // Aquí podés actualizar el PC o lo que corresponda
    break;

    case IO_SYS:
    log_info(data->logger, "Ejecutando IO: dispositivo=%s, tiempo=%d",
      instruccion_completa->args[0], atoi(instruccion_completa->args[1]));
    // Simular llamada IO (bloqueo)
    break;

    case INIT_PROC_SYS:
    log_info(data->logger, "Ejecutando INIT_PROC: nombre=%s, pid=%d",
      instruccion_completa->args[0], atoi(instruccion_completa->args[1]));
    // Inicializar proceso
    break;

    case DUMP_MEMORY_SYS:
    log_info(data->logger, "Ejecutando DUMP_MEMORY");
    // Mostrar memoria o lo que haga Dump Memory
    break;

    case EXIT_SYS:
    log_info(data->logger, "Ejecutando EXIT");
    // Terminar proceso, liberar recursos
    break;

    default:
    log_error(data->logger, "Instrucción inválida");
      // case NOOP:
      //   ejecutar_noop(); 
      //   break;
      // case WRITE:        
      // ejecutar_write(instruccion_completa->args[0], instruccion_completa->args[1]);
      //   break;
      // case READ:        
      //   ejecutar_read(instruccion_completa->args[0]); 
      //   break;
      // case GOTO:       
      // ejecutar_goto(atoi(instruccion_completa->args[0]));
      //   break;
      // case IO_SYS:
      //   ejecutar_io(instruccion_completa->args[0], atoi(instruccion_completa->args[1])); 
      //   break;
      // case INIT_PROC_SYS:
      // ejecutar_init_proc(instruccion_completa->args[0], atoi(instruccion_completa->args[1]));
      //   break;
      // case DUMP_MEMORY_SYS:
      //   ejecutar_dump_memory(); 
      //   break;
      // case EXIT_SYS:
      //   ejecutar_exit();
      //   break;
      // default:           
      //   log_error(data->logger, "Instrucción inválida");
    }
 

    free(instruccion_completa);
    close(socket_memoria); // conexión efímera con Memoria
    free(pcb);
    free(tbuffer->stream);
    free(tbuffer);

  }

  free(data);
  pthread_exit(NULL);
}

void* conectar_kernel_interrupt(void* args) {
  argumentos_conexion* data = (argumentos_conexion*)args;
  int socket_dispatch = crear_conexion(data->logger, data->ip, data->puerto);
  if (socket_dispatch == -1) {
    log_error(data->logger, "Fallo la conexión con INTERRUPT.");
    pthread_exit(NULL);
  }

  log_info(data->logger, "Conectado a INTERRUPT en %s:%d", data->ip, data->puerto);
  log_info(data->logger, "%s conectada. Esperando peticiones...", data->identificador);
  char* handshake = data->identificador;
  uint32_t size = strlen(handshake) + 1;
  send(socket_dispatch, &size, sizeof(uint32_t), 0);
  send(socket_dispatch, handshake, size, 0);
  log_info(data->logger, "Handshake enviado con ID: %s", data->identificador);
  log_info(data->logger, "Conexión y handshake exitosos");

  return NULL;
}


instruccion_t_completa* parsear_instruccion(const char* linea_original) {
  char linea[MAX_LINEA];
  strncpy(linea, linea_original, MAX_LINEA);
  linea[strcspn(linea, "\r\n")] = '\0';

  char* tokens[MAX_ARGS + 1];
  int ntok = 0;
  char* tok = strtok(linea, " ");
  while (tok && ntok < MAX_ARGS + 1) {
    tokens[ntok++] = tok;
    tok = strtok(NULL, " ");
  }

  instruccion_t_completa* instr = malloc(sizeof(instruccion_t_completa));
  if (!instr) return NULL;

  instr->tipo = instruccion_from_str(tokens[0]);
  instr->nargs = ntok - 1;

  for (int i = 0; i < instr->nargs; i++){
    strncpy(instr->args[i], tokens[i + 1], MAX_LINEA);
    instr->args[i][MAX_ARG_LENGTH - 1] = '\0'; // asegurar fin de cadena
  }

  return instr;
}

instruccion_t instruccion_from_str(const char* str) {
  if (strcmp(str, "NOOP") == 0) return NOOP;
  if (strcmp(str, "WRITE") == 0) return WRITE;
  if (strcmp(str, "READ") == 0) return READ;
  if (strcmp(str, "GOTO") == 0) return GOTO;
  if (strcmp(str, "IO_SYS") == 0) return IO_SYS;
  if (strcmp(str, "INIT_PROC_SYS") == 0) return INIT_PROC_SYS;
  if (strcmp(str, "DUMP_MEMORY_SYS") == 0) return DUMP_MEMORY_SYS;
  if (strcmp(str, "EXIT_SYS") == 0) return EXIT_SYS;
}

const char* instruccion_to_string(instruccion_t tipo) {
  switch (tipo) {
    case NOOP: return "NOOP";
    case WRITE: return "WRITE";
    case READ: return "READ";
    case GOTO: return "GOTO";
    case IO_SYS: return "IO";
    case INIT_PROC_SYS: return "INIT_PROC";
    case DUMP_MEMORY_SYS: return "DUMP_MEMORY";
    case EXIT_SYS: return "EXIT";
    default: return "INSTRUCCION_INVALIDA";
  }
}