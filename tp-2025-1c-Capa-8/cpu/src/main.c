#include <utils/hello.h>
#include <pthread.h>
#include <stdio.h>
#include "utils/protocolo.h"
#include "cpu.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s [identificador_cpu]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* identificador_cpu = argv[1];
    t_log* cpu_log = inicializar_cpu_log(identificador_cpu);
    t_config* cpu_config = inicializar_cpu_config();

    pthread_t hilo_dispatch, hilo_interrupt;
    argumentos_conexion* argumentos_dispatch = cargar_argumentos(IP_KERNEL, cpu_log, PUERTO_KERNEL_DISPATCH, identificador_cpu);
    pthread_create(&hilo_dispatch, NULL, conectar_kernel_dispatch, argumentos_dispatch);

    argumentos_conexion* interrupt = cargar_argumentos(IP_KERNEL, cpu_log, PUERTO_KERNEL_INTERRUPT, identificador_cpu);
    pthread_create(&hilo_interrupt, NULL, conectar_kernel_interrupt, interrupt);


    // log_info(cpu_log, "configuraciones leidas correctamente");

    // // -------- Conectarse al Kernel (dispatch + interrupt) -------- //
    // int socket_dispatch = crear_conexion(cpu_log, IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    // if (socket_dispatch <= 0) {
    //     log_error(cpu_log, "Fallo conexión al Kernel (DISPATCH) en %s:%d", IP_KERNEL, PUERTO_KERNEL_DISPATCH);
    //     return EXIT_FAILURE;
    // }
    // int socket_interrupt = crear_conexion(cpu_log, IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    // if (socket_interrupt <= 0) {
    //     log_error(cpu_log, "Fallo conexión al Kernel (INTERRUPT) en %s:%d", IP_KERNEL, PUERTO_KERNEL_INTERRUPT);
    //     return EXIT_FAILURE;
    // }

    // // // -------- Handshake con Kernel por ambos sockets -------- //
    // enviar_handshake(cpu_log, socket_dispatch, identificador_cpu);
    // enviar_handshake(cpu_log, socket_interrupt, identificador_cpu);
    // log_info(cpu_log, "Conexión y handshake con Kernel exitosa");

    // Crear hilos
    // pthread_t t_dispatch, t_interrupt;
    // pthread_create(&t_dispatch, NULL, hilo_dispatch, (void *)&socket_dispatch);
    // pthread_create(&t_interrupt, NULL, hilo_interrupt, (void *)&socket_interrupt);

    // pthread_join(t_dispatch, NULL);
    // pthread_join(t_interrupt, NULL);
    // TODO:Iniciar ciclo de instrucción (fetch-decode-execute)
    
    // log_info(logger,"mensjae1");
    // sleep(1);
    // log_info(logger,"mensjae2");
    // enviar_mensaje(momo, socket_dispatch);
    
    
    // shutdown(sock_dispatch,SHUT_RDWR);
    
    /*int sock_memoria = conectar(ip_memoria, puerto_memoria);
    send(sock_memoria, &size, sizeof(uint32_t), 0 );
    send(sock_memoria, id, size, 0);*/
    // shutdown(sock_memoria,SHUT_RDWR);

    /*int sock_interrupt = conectar(ip_kernel, puerto_interrupt);
    send(sock_interrupt, &size, sizeof(uint32_t), 0);
    send(sock_interrupt, id, size, 0);*/
    // shutdown(sock_interrupt,SHUT_RDWR);
    

   // log_info(logger, "CPU '%s' conectada al Kernel por dispatch (%d) e interrupt (%d)", id, puerto_dispatch, puerto_interrupt);

    //while (1) pause();

    pthread_join(hilo_dispatch, NULL);
    pthread_join(hilo_interrupt, NULL);

    config_destroy(cpu_config);
    log_destroy(cpu_log);

    return 0;
}
