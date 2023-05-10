#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char *tmp = _data;

    memcpy(tmp, &type, sizeof(uint8_t));

    tmp += sizeof(uint8_t);

    memcpy(tmp, nick.c_str(), 8 * sizeof(char));

    tmp += 8 * sizeof(char);

    memcpy(tmp, message.c_str(), 80 * sizeof(char));

    //std::cout << "SerializedMessage: " << _data << std::endl;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data

    char *tmp = _data;

    memcpy(&type, tmp, sizeof(uint8_t));

    tmp += sizeof(uint8_t);

    nick = tmp;

    tmp += 8 * sizeof(char);

    message = tmp;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        ChatMessage em;

        Socket *client;

        socket.recv(em, client);

        switch (em.type)
        {
            case ChatMessage::LOGIN:
            {
                std::unique_ptr<Socket> client_ptr(client);

                clients.push_back(std::move(client_ptr));

                std::cout << "Client " << em.nick << " connected" << std::endl;

                break;
            }
            case ChatMessage::LOGOUT:
            {
                auto it = clients.begin();

                while (it != clients.end())
                {
                    if (*it->get() == *client)
                    {
                        std::cout << "Client " << em.nick << " disconnected" << std::endl;

                        clients.erase(it);

                        break;
                    }

                    ++it;
                }

                break;
            }
            case ChatMessage::MESSAGE:
            {
                auto it = clients.begin();

                while (it != clients.end())
                {
                    if (*it->get() != *client)
                    {
                        socket.send(em, *(*it));
                    }

                    ++it;
                }

                break;
            }
            default:
            {
                std::cout << "Unknown message type" << std::endl;

                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar

    std::string msg;

    ChatMessage em(nick, msg);

    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);

}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket

        std::string msg;

        std::getline(std::cin, msg);

        ChatMessage em(nick, msg);

        em.type = ChatMessage::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"

        ChatMessage em;

        socket.recv(em);

        std::cout << em.nick << ": " << em.message << std::endl;


    }
}

