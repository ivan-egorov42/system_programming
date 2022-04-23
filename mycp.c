#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

const int SIZE = 10000;
extern char* optarg;
extern int optind, opterr, optopt;

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("Too less arguments\n");
		exit(-1);
	}   
	char* optstr = "vfi";
	int status = 0;
	int f_status = 0;

    int i = 0, f = 0, v = 0;
	while((status = getopt(argc, argv, optstr)) != -1)
	{
		switch(status)
		{
			case 'i':
				i = 1;
				break;
			case 'f':
				f_status = 1;
				break;
				
			case 'v':
                v = 1;
				break;
			case ':':
				printf("Need argument\n");
				exit(-1);
			case '?':
				printf("Wrong option\n");
				exit(-1);
			default:
				exit(-1);
		}
	}

    if (v == 1)
    {
        if(argc - 1 < optind + 1)
		{
			printf("Too less arguments\n");
			exit(-1);
		}
		printf("'%s' -> '%s'\n", argv[optind], argv[optind+1]);
    }

    if (i == 1)
    {
        int fd = 0;
        if(argc - 1 < optind + 1)
				{
                    printf("%d\n", optind);
					printf("Too less arguments\n");
					exit(-1);
				}
				if((fd = open(argv[optind+1], O_WRONLY)) >= 0)
				{
					printf("Overwrite %s?\n", argv[optind+1]);
					char ans;
					scanf("%c", &ans);
					if(ans != 'y')
                    {
						exit(0);
                        close(fd);
                    }
				}
    }

    if(f_status)
    {
        int fd2 = open(argv[optind + 1], O_RDWR | O_CREAT, 0777);
        if (fd2 < 0)
        {
            unlink(argv[optind + 1]);
        }

        close(fd2);
    }

    int fd1 = open(argv[optind], O_RDONLY);
	int fd2 = open(argv[optind+1], O_TRUNC | O_CREAT | O_WRONLY, 0777);

	char* buf = (char*) calloc(SIZE, sizeof(char));
	int num;

	while((num = read(fd1, buf, SIZE)) > 0)
    {
		write(fd2, buf, num);
    }
	close(fd1);
	close(fd2);

	return 0;
}