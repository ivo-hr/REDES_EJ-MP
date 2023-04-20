#include <iostream>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <time.h>

using namespace std;
int main (int argc, char** argv)
{
    if (argc != 3){
        cout << "Usage: " << "x.x.x.x" << " host port " << endl;
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        cout << "Error from [getaddrinfo]: " << gai_strerror(rc) << endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sd < 0)
    {
        cout << "Error from [socket]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        return -1;
    }

    int bd = bind(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);

    if (bd < 0)
    {
        cout << "Error from [bind]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    bd = listen(sd, 1);

    if (bd == -1)
    {
        cout << "Error from [listen]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    char buffer[80];

    sockaddr client;
    socklen_t client_len = sizeof(sockaddr);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    while(true){
        int cd = accept(sd, (struct sockaddr *) &client, &client_len);

        if (cd < 0)
        {
            cout << "Error from [accept]: " << strerror(errno) << endl;
            freeaddrinfo(res);
            close(sd);
            return -1;
        }

        getnameinfo(&client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        cout << "Connection from " << host << " " << serv << endl;

        int bytes = recv(cd, buffer, strlen(buffer), 0);

        while (bytes != -1){
            if (bytes == 0) break;
            else buffer[bytes] = '\0';

            bytes = recv(cd, buffer, strlen(buffer), 0);
        }

        cout << "Ended connection from " << host << " " << serv << endl;

        close(cd);
    }

    return 0;
}