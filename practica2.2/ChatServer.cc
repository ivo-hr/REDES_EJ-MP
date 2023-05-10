#include "Chat.h"
#include <signal.h>


int main(int argc, char **argv)
{
    sigset_t set;
    int s;

    ChatServer es(argv[1], argv[2]);

    es.do_messages();

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    sigwait(&set, &s);

    return 0;
}
