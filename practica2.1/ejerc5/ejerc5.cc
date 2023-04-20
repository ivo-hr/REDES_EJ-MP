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
    hints.ai_socktype = SOCK_STREAM;
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

    int cd = connect(sd, res->ai_addr, res->ai_addrlen);

    if (cd == -1)
    {
        cout << "Error from [connect]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    string buffer;
    char rec[80];

    sockaddr client;
    socklen_t client_len = sizeof(sockaddr);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    while(true){
        cin >> buffer;

        if (buffer == "Q"){
            cout << "Terminating connection..." << endl;
            freeaddrinfo(res);
            close(sd);
            break;
        }

        int s = send(sd, buffer.c_str(), buffer.size(), 0);

        int r = recv(sd, rec, 80, 0);
        if (r > -1 && r < 80){
            rec[r] = '\0';
        }

        cout << rec << endl;
    }

    return 0;
}