#include <utils/hello.h>

void saludar(char* quien) {
    printf("Hola desde %s!!\n", quien);
};
#include <utils/hello.h>


int iniciar_servidor(t_log* logger, int puerto){
	int socket_servidor;
	struct addrinfo hints, *servinfo;
    
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

    char puerto_str[6];  // hasta 65535 + '\0'
    snprintf(puerto_str, sizeof(puerto_str), "%d", puerto);
    int err = getaddrinfo(NULL, puerto_str, &hints, &servinfo);
    if (err != 0) {
        log_error(logger, "Error en getaddrinfo %s", gai_strerror(err));
    }

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	log_info(logger, "Listo para escuchar a mi cliente en puerto: %d", puerto);

	freeaddrinfo(servinfo);

	return socket_servidor;
}

int esperar_cliente(t_log* logger, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
    if (socket_cliente == -1) {
		log_error(logger, "Error al aceptar cliente en %d", socket_servidor);
		return -1;
	}

    // HANDSHAKE
    uint32_t len;
    if (recv(socket_cliente, &len, sizeof(uint32_t), MSG_WAITALL) <= 0) {
        log_error(logger, "Error recibiendo longitud del identificador");
        return -1;
    }

    char* identificador = malloc(len);
    if (recv(socket_cliente, identificador, len, MSG_WAITALL) <= 0) {
        log_error(logger, "Error recibiendo identificador");
        free(identificador);
        return -1;
    }
    
    log_info(logger, "## %s Conectado - FD del socket %d", identificador, socket_cliente);

    free(identificador);
    return socket_cliente;
}


int crear_conexion(t_log* logger, char* ip, int puerto) {
    struct addrinfo hints, *servinfo;

    // Init de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // <-- solo si es de escucha

    // Recibe addrinfo
    char puerto_str[6];  // hasta 65535 + '\0'
    snprintf(puerto_str, sizeof(puerto_str), "%d", puerto);
    int err = getaddrinfo(ip, puerto_str, &hints, &servinfo);
    if(err != 0){
        log_error(logger, "Error en getaddrinfo %s", gai_strerror(err));
    }

    // Crea un socket con la informacion recibida (del primero, suficiente)
    int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Fallo en crear el socket
    if(socket_cliente == -1) {
        log_error(logger, "Error creando el socket para %s:%d", ip, puerto);
        return -1;
    }

    // Error conectando
    if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        log_error(logger, "Error al conectar con %s:%d", ip, puerto);
        freeaddrinfo(servinfo);
        return -1;
    } else{
        log_info(logger, "Cliente conectado en %s:%d", ip, puerto);
    }

    freeaddrinfo(servinfo); // LIBERAR MEMORIA

    return socket_cliente;
}

// CERRAR CONEXION
void liberar_conexion(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}

// HANDSHAKE
void enviar_handshake(t_log* logger, int socket_cliente, const char* identificador) {
    uint32_t len = strlen(identificador) + 1;  // +1 para el '\0'

    if (send(socket_cliente, &len, sizeof(uint32_t), 0) <= 0) {
        perror("Error enviando largo del identificador");
        exit(EXIT_FAILURE);
    }

    if (send(socket_cliente, identificador, len, 0) <= 0) {
        perror("Error enviando identificador");
        exit(EXIT_FAILURE);
    }

    log_info(logger, "Handshake enviado con ID: %s", identificador);
    log_info(logger, "Conexión y handshake exitosos");
}