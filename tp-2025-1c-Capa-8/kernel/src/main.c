#include "kernel.h"
#include <utils/hello.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

// int conectar(char* ip, int puerto);
void* conectarconmemoria(void* args);

int main(int argc, char* argv[]) {
    if(argc != 3){
        printf("Uso: %s <archivo_pseudocodigo> <tamanio_proceso>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* path_pseudocodigo = argv[1];
    int tamanio_proceso = atoi(argv[2]);
    
    // inicializacion de Log y Config    
    t_log* kernel_log = inicializar_kernel_log();
    t_config* kernel_config = inicializar_kernel_config();     

    //inicializar listas
    cola_new = list_create();
    cola_ready = list_create();
    cola_exec = list_create();
    cola_blocked = list_create();
    cola_exit = list_create();
    
    //inicializar mutex

    pthread_mutex_init(&mutex_cola_new, NULL);
    pthread_mutex_init(&mutex_cola_ready, NULL);
    pthread_mutex_init(&mutex_cola_exec, NULL);
    pthread_mutex_init(&mutex_cola_blocked, NULL);
    pthread_mutex_init(&mutex_cola_exit, NULL);
    pthread_mutex_init(&mutex_cola_io, NULL);
    //inicializar semaforos
    sem_init(&sem_procesos_en_ready, 0, 0);
    sem_init(&sem_multiprogramacion, 0,0);

    //crear hilos de planificacion
    pthread_t hilo_planificador;
    pthread_create(&hilo_planificador, NULL, planificador_corto_plazo, NULL);

    //crear proceso
    // for(int i=0;i<5;i++) {
    //     pcb_t* proceso = crear_proceso();
    //     mover_a_ready(proceso);
    // }
    
    pthread_t hilo_memoria;
    argumentos_conexiones* memoria = cargar_memoria(IP_MEMORIA,kernel_log,PUERTO_MEMORIA);
    pthread_create(&hilo_memoria,NULL, conectarconmemoria, argumentos_memoria);
    
    pthread_t hilo_dispatch;
    argumentos_conexiones* dispatch = cargar_argumentos(kernel_log,PUERTO_ESCUCHA_DISPATCH);
    pthread_create(&hilo_dispatch,NULL, crear_hilo_modulo,dispatch);

    pthread_t hilo_interrupt;
    argumentos_conexiones* interrupt = cargar_argumentos(kernel_log,PUERTO_ESCUCHA_INTERRUPT);
    pthread_create(&hilo_interrupt,NULL, crear_hilo_modulo,);

    pthread_t hilo_IO;
    argumentos_conexiones* IO = cargar_argumentos(kernel_log,PUERTO_ESCUCHA_IO);
    pthread_create(&hilo_IO, NULL, crear_hilo_modulo,IO);

    pthread_join(hilo_memoria,NULL);
    pthread_join(hilo_interrupt,NULL);
    pthread_join(hilo_dispatch,NULL);
    pthread_join(hilo_IO,NULL);
    // SOCKET CONEXION MEMORIA
    ////int socket_memoria_cliente = crear_conexion(kernel_log, IP_MEMORIA, PUERTO_MEMORIA);
    // HANDSHAKE
    ////char* identificador = "Kernel";
    ////enviar_handshake(kernel_log, socket_memoria_cliente, identificador);

    // SOCKET SERVIDOR PARA CPU/IO
    
    ////int socket_interrupt_server = iniciar_servidor(kernel_log, PUERTO_ESCUCHA_INTERRUPT);
    ////int socket_io_server = iniciar_servidor(kernel_log, PUERTO_ESCUCHA_IO);

    // ESPERAR A QUE SE CONECTEN
    ////int socket_dispatch_cliente = esperar_cliente(kernel_log, socket_dispatch_server);
    ////int socket_interrupt_cliente = esperar_cliente(kernel_log, socket_interrupt_server);
    ////int socket_io_cliente = esperar_cliente(kernel_log, socket_io_server);


    // printf("hola %s",puerto_dispatch);    
    //log_info(logger, "lei %d", puerto_dispatch);
    //------------------------------
    //int socket_servidor = iniciar_servidor(logger, ip_memoria, puerto_memoria);
    ///esperar_cliente(logger,"memoria",soket_servidor);
    //crear_conexion(logger, ip_memoria, puerto_memoria);
    //------------------------------
    //int sokert_dispatch_cpu=  iniciar_servidor(logger, ip_CPU, puerto_dispatch);
    //int socket_dispatch = esperar_cliente(logger, "Cpu", socket_dispatch_cpu);
    //crear_conexion(logger,ip_CPU,puerto_dispatch);
    // recibir_mensaje(logger, socket_dispatch);
    // recibir_mensaje(logger, socket_dispatch);

    // log_info(logger, "Kernel esta activo %d");
    // log_info(logger, puerto_io);
   // pthread_t hilo_memoria;
    
    //------------------------------------------------------------------------------------------
    /* MODULO MEMORIA */
    /*pthread_create(&hilo_memoria,NULL,conectarconmemoria,data);
    pthread_join(hilo_memoria,NULL);
    free(data);*/

    
    //int servidor_dispatch = iniciar_servidor(puerto_dispatch, logger);
    //int servidor_interrupt = iniciar_servidor(puerto_interrupt, logger);
    
    /*if (servidor_dispatch == -1) {
        log_error(logger, "No se pudo iniciar servidor");
        return EXIT_FAILURE;
    }*/

    //aceptar_conexion(servidor_dispatch, logger, "DISPATCH");
    //aceptar_conexion(servidor_interrupt, logger, "INTERRUPT");
    //esperar_conexion(puerto_io,"IO", logger);
    config_destroy(kernel_config);
    log_destroy(kernel_log);
    
    return 0;
} 

    


// int conectar(char* ip, int puerto) {

//     int socket_io = socket(AF_INET, SOCK_STREAM, 0);
//     if (socket_io == -1)
//       return -1;


//     struct sockaddr_in direccion;
//     direccion.sin_family = AF_INET;
//     direccion.sin_port = htons(puerto);
//     direccion.sin_addr.s_addr = inet_addr(ip);

//     if (connect(socket_io, (void *)&direccion, sizeof(direccion)) != 0)
//     {
//         close(socket_io);
//         return -1;
//     }

//     return socket_io;
// };
argumentos_conexiones* cargar_argumentos(char* ip,t_log* logger, int  puerto){
    argumentos_conexiones* data = malloc(sizeof(argumentos_conexiones));
    data->logger = logger;
    data->puerto = puerto;
    return data;
}

argumentos_memoria* cargar_memoria(char* ip,t_log* logger, int  puerto){
    argumentos_conexiones* data = malloc(sizeof(argumentos_memoria);
    data->ip= ip;
    data->logger = logger;
    data->puerto = puerto;
    return data;
}

void* crear_hilo_modulo(void* arg){
    argumentos_conexiones* args = (argumentos_conexiones*) arg;
    int socket_modulo = iniciar_servidor(args->logger, args->puerto);
    if (socket_modulo == -1) {
        log_error(args->logger, "Fallo la conexión con el modulo.");
        return EXIT_FAILURE;
    }
    int socket_dispatch_server = iniciar_servidor(args->logger, args->puerto);
    int socket_dispatch_cliente = esperar_cliente(args->logger, socket_dispatch_server);
    //poner swich
}

void* conectarconmemoria(void* arg){ 
    argumentos_memoria* args = (argumentos_memoria*) arg;
    int socket_memoria = crear_conexion(args->ip, args->puerto);
    if (socket_memoria == -1) {
        log_error(args->logger, "Fallo la conexión con la memoria.");
        return EXIT_FAILURE;
    }

    log_info(args->logger, "Conectado al memoria en %s:%d", "ip_memoria", args->puerto);
    log_info(args->logger, "Memoria conectada. Esperando peticiones...");
    char* handshake = "KERNEL";
    uint32_t size = strlen(handshake) + 1;
    send(socket_memoria, &size, sizeof(uint32_t), 0);
    send(socket_memoria, handshake, size, 0);
    log_info(args->logger, "Handshake enviado con nombre: %s", "kernel");

    ///poner swich case
}

