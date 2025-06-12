#include "utils/hello.h"
#include "memoria.h"

int server_escuchar(t_log* logger, int socket_servidor);

int main(int argc, char* argv[]) {
    t_log* memoria_log = inicializar_log();
    t_config* memoria_config = inicializar_config();


    pthread_t hilo_servidor_memoria;
    argumentos_servidor_memoria* arg = cargar_argumentos(memoria_log, PUERTO_MEMORIA);
    pthread_create(&hilo_servidor_memoria, NULL, atender_cliente, arg);

    pthread_join(hilo_servidor_memoria, NULL);

    inicializar_estructuras ();

    

    
    // SOCKET SERVIDOR
    // int socket_memoria = iniciar_servidor(memoria_log, PUERTO_MEMORIA);
    // server_escuchar(memoria_log, socket_memoria);

    
    // esperar_conexion(puerto, "MEMORIA_KERNEL", logger);
    // shutdown(puerto,SHUT_RDWR);
    // esperar_conexion(puerto, "MEMORIA_CPU", logger);
   

    /*int servidor = iniciar_servidor(puerto, logger);

    if (servidor == -1) {
        log_error(logger, "No se pudo iniciar servidor");
        return EXIT_FAILURE;
    }

    while (1) {
        aceptar_conexion(servidor, logger, "MEMORIA");
    }*/

    


    // int servidor = socket(AF_INET, SOCK_STREAM, 0);
    // struct sockaddr_in direccion;
    // direccion.sin_family = AF_INET;
    // direccion.sin_addr.s_addr = INADDR_ANY;
    // direccion.sin_port = htons(puerto);

    // bind(servidor, (void*)&direccion, sizeof(direccion));
    // listen(servidor, 10);

    // log_info(logger, "Memoria escuchando en puerto %d", puerto);
   
    // while(1) {
    //     log_info(logger, "me meti en el while %d", puerto);
        


    //     int cliente = accept(servidor, NULL, NULL);
    //     log_info(logger, "Nueva conexion aceptada");

    // }

    config_destroy(memoria_config);
    log_destroy(memoria_log);

    return 0;
}


int server_escuchar(t_log* logger, int socket_servidor) {
    //char* server_name = "Memoria"; // <-- no se usa
    int socket_cliente = esperar_cliente(logger, socket_servidor);

    // if (cliente_socket != -1) {
    //     //pthread_t hilo;
    //     int *args = malloc(sizeof(int));
    //     args = &cliente_socket;
    //     //pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
    //     //pthread_detach(hilo);
    //     return 1;
    //}
    return socket_cliente;
}

