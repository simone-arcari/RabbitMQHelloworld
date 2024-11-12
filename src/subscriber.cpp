#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <iostream>
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

    // Dichiarazione della coda (deve corrispondere a quella del Publisher)
    amqp_queue_declare(conn, 1, amqp_cstring_bytes("test_queue"), 0, 0, 0, 1, amqp_empty_table);
    amqp_get_rpc_reply(conn);

    // Inizia a consumare i messaggi
    amqp_basic_consume(conn, 1, amqp_cstring_bytes("test_queue"), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    amqp_get_rpc_reply(conn);

    while (true) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn);
        res = amqp_consume_message(conn, &envelope, nullptr, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }

        std::cout << "Ricevuto messaggio: " << std::string((char*)envelope.message.body.bytes, envelope.message.body.len) << std::endl;
        amqp_destroy_envelope(&envelope);
    }

    // Chiudi il canale e la connessione
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

    return 0;
}

