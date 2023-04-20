#include <iostream>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <time.h>

#include <thread>

using namespace std;

class ServerThread
{
    public:
    ServerThread(int sd) : _sd(sd)
    {
        _thread = thread([this](){this->GetMsg();});
    }

    ~ServerThread()
    {
        _thread.detach();
    }

    void GetMsg()
    {
        char buffer[80];
        char resp[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        sockaddr client;
        socklen_t client_len = sizeof(sockaddr);

        while (true){
            int bytes = recvfrom(_sd, buffer, 80, 0, &client, &client_len);

            if (bytes < 0)
            {
                cout << "Error from [recvfrom]: " << strerror(errno) << endl;
                continue;
            }

            if(buffer[bytes - 1] == '\n')
                buffer[bytes - 1] = '\0';
            else
                buffer[bytes] = '\0';

            getnameinfo(&client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            cout << "Recieved " << bytes << " bytes from host " << host << ":" << serv << endl;

            time_t t = time(NULL);
            time(&t);
            struct tm* timeinfo = localtime(&t);
            size_t datelen;

            if (strcmp(buffer, "t") == 0)
            {
                datelen = strftime(resp, 80, "%r", timeinfo);
            }
            else if (strcmp(buffer, "d") == 0)
            {
                datelen = strftime(resp, 80, "%F", timeinfo);
            }
            else if (strcmp(buffer, "q") == 0)
            {
                cout << "Closing connection with " << host << ":" << serv << endl;
                close(_sd);
                break;
            }
            else
            {
                cout << "Unknown command " << buffer << endl;
                continue;
            }

            sleep(3);
            cout << this_thread::get_id() << " sending " << resp << endl;
            sendto(_sd, resp, datelen, 0, &client, client_len);
        }
    }

    private:
    int _sd;
    thread _thread;
};

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

    int bd = bind(sd,(struct sockaddr *) res->ai_addr, res->ai_addrlen);

    if (bd < 0)
    {
        cout << "Error from [bind]: " << strerror(errno) << endl;
        freeaddrinfo(res);
        close(sd);
        return -1;
    }

    ServerThread* st[5];

    for (int i = 0; i < 5; i++)
    {
        st[i] = new ServerThread(sd);
    }

    for (int i = 0; i < 5; i++)
    {
        delete st[i];
    }

    freeaddrinfo(res);
    close(sd);
    return 0;

}