#include <utils/hello.h>
#include "io.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s [nombre_dispositivo]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* identificador_io = argv[1];
    t_log* io_log = inicializar_io_log();
    t_config* io_config = inicializar_io_config();

    // Conectarse al Kernel
    int socket_conexion_kernel = crear_conexion(io_log, IP_KERNEL, PUERTO_KERNEL);
    // HANDSHAKE
    enviar_handshake(io_log, socket_conexion_kernel, identificador_io);

    log_info(io_log, "Conexión y handshake con kernel exitosa");


    // Simulación básica: espera eterna
    // log_info(logger, "IO listo. Esperando peticiones...");
    // while (1) pause();
    
    // log_info(logger, "Paso el while");


    // // Cleanup (nunca llega acá pero por convención)
    // close(socket_kernel);
    config_destroy(io_config);
    log_destroy(io_log);

    return EXIT_SUCCESS;
}
