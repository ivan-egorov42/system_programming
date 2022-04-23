#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const size_t SIZE = 256;
const int stdin  = 0;
const int stdout = 1;
const int stderr = 2;

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        char buf[SIZE];
        while(1)
        {
            int size = read(stdin, buf, SIZE);
            if (size == 0) 
                break;
            mywrite(&size, buf, stdout);

        }

    }
}

void mywrite(int* size, char* buf, int stream)
{
    while(*size) 
        *size = write(stream, buf, *size);
}