#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    const char* hostname = "localhost";
    int port = 5672;

    amqp_connection_state_t conn = amqp_new_connection();
    amqp_socket_t* socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        std::cerr << "Errore nella creazione del socket TCP.\n";
        return 1;
    }

    if (amqp_socket_open(socket, hostname, port)) {
        std::cerr << "Errore di connessione a RabbitMQ.\n";
        return 1;
    }

    // Login
    amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    // Dichiarazione della coda
    amqp_queue_declare(conn, 1, amqp_cstring_bytes("test_queue"), 0, 0, 0, 1, amqp_empty_table);
    amqp_get_rpc_reply(conn);

    // Invia un messaggio alla coda
    std::string message;
    while (true) {
        std::cout << "Inserisci il messaggio da inviare (o 'exit' per uscire): ";
        std::getline(std::cin, message);
        if (message == "exit") break;

        amqp_bytes_t message_bytes = amqp_cstring_bytes(message.c_str());
        amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes("test_queue"),
                           0, 0, nullptr, message_bytes);
        std::cout << "Messaggio inviato: " << message << std::endl;
    }

    // Chiudi il canale e la connessione
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

    return 0;
}

