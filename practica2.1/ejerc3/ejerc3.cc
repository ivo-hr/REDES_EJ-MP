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
    if (argc != 4){
        cout << "Usage: " << "x.x.x.x" << " host port " << " command " << endl;
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

    int bd = connect(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);

    if (bd < 0)
    {
        cout << "Error from [connect]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    char msg[1024];
    strcpy(msg, argv[3]);
    msg[1023] = '\0';

    sendto(sd, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);

    char buffer[1024];
    int bytes = recvfrom(sd, (void *) buffer, 1024, 0, res->ai_addr, &res->ai_addrlen);
    buffer[bytes] = '\0';
    cout << buffer << endl;

    freeaddrinfo(res);
    close(sd);
    return 0;
}
