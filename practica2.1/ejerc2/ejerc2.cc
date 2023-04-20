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
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " host port" << endl;
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
        return -1;
    }

    int bd = bind(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);

    if (bd < 0)
    {
        cout << "Error from [bind]: " << strerror(errno) << endl;
        return -1;
    }
    else if (bd == 1){
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    char buffer[80];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    char resp[80];

    sockaddr client;
    socklen_t client_len = sizeof(sockaddr);

    while(true){
        int bytes = recvfrom(sd, (void *) buffer, 80, 0, &client, &client_len);

        if (bytes < 0)
        {
            cout << "Error from [recvfrom]: " << strerror(errno) << endl;
            continue;
        }

        if(buffer[bytes - 1] == '\n')
            buffer[bytes - 1] = '\0';
        else
            buffer[bytes] = '\0';

        int cd = getnameinfo(&client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        
        if (cd != 0)
        {
            cout << "Error from [getnameinfo]: " << gai_strerror(cd) << endl;
            return -1;
        }

        cout << "Recieved " << bytes << " bytes from host " << host << ":" << serv << endl;


        time_t t = time(NULL);
        time(&t);
        tm * time = localtime(&t);
        size_t datelen;

        if (strcmp(buffer, "t") == 0)
        {
            datelen = strftime(resp, 80, "%I:%M:%S %p", time);
        }
        else if (strcmp(buffer, "d") == 0)
        {
            datelen = strftime(resp, 80, "%Y-%m-%d", time);
        }
        else if (strcmp(buffer, "q") == 0)
        {
            cout << "Exiting..." << endl;
            break;
        }
        else
        {
            cout << "Command not recognized" << endl;
            continue;
        }

        sendto(sd, resp, datelen, 0, &client, client_len);

    }

    freeaddrinfo(res);
    return 0;
}
