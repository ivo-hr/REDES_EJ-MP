#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //
        int data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* tmp = _data;

        memcpy(tmp, name, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);

        memcpy(tmp, &pos_x, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(tmp, &pos_y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        //

        char* tmp = data;

        memcpy(name, tmp, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);

        memcpy(&pos_x, tmp, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(&pos_y, tmp, sizeof(int16_t));


        return 0;
    }


private:
    static const size_t MAX_NAME = 20;

    int16_t pos_x;
    int16_t pos_y;

    char name[MAX_NAME];


};



int main(int argc, char **argv)
{
    //Jugador one_r("", 0, 0);
    Jugador one("Player_ONE", 123, 987);

    one.to_bin();

    int file = open("player", O_CREAT | O_WRONLY, 0666);
    write(file, one.data(), one.size());
    close(file);


    return 0;
}


