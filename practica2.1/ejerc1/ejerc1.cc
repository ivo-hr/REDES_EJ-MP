#include <iostream>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int main (int argc, char** argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    int rc = getaddrinfo(argv[1], NULL, &hints, &res);

    if (rc != 0)
    {
        std::cerr << "Error from [getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    for (auto i = res; i != NULL; i = i->ai_next)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        int cd = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        
        if (cd != 0)
        {
            std::cerr << "Error from [getnameinfo]: " << gai_strerror(cd) << std::endl;
            return -1;
        }

        std::cout << host << " " << i->ai_family << " " << i->ai_socktype << std::endl;
    }

    freeaddrinfo(res);

    return 0;
}
