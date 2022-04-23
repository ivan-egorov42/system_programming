#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int captain (int sems_id, int n_seats, int n_rides);
int passenger (int sems_id, int passenger_no, int n_seats);


int main (int argc, char** argv)
{
	printf ("~Boat is ready to get passengers\n");

	if (argc != 4)
	{
		printf ("Please enter 3 arguments: seats, passengers and rides\n");
		return 1;
	}

	int n_passengers = 0;
	int n_seats = 0;
	int n_rides = 0;
	sscanf (argv[1], "%d", &n_seats);
	sscanf (argv[2], "%d", &n_passengers);
	sscanf (argv[3], "%d", &n_rides);

	int sems_id = semget (IPC_PRIVATE, 3, 0666);
    unsigned short init_array[3] = {n_seats, 3, 1};
    semctl (sems_id, 0, SETALL, init_array);

    int captain_pid = fork ();

    if (captain_pid == 0)
    {
    	return captain (sems_id, n_seats, n_rides);
    }

    for (int i = 1; i < n_passengers + 1; i++)
    {
    	int passenger_pid = fork ();

    	if (passenger_pid == 0)
    		return passenger (sems_id, i, n_seats);
    }

	for (int i = 0; i < n_passengers + 1; i++)
		wait (NULL);


	printf ("~Boat is empty\n");
	return 0;
}

// Semaphore[0] - seats on boat
// Semaphore[1] - trap
// Semaphore[2] - boarding

int captain (int sems_id, int n_seats, int n_rides)
{
	struct sembuf begin[2] = {{0, 0, 1}, {2, 1, 0}};
	struct sembuf end[2] = {{0, -n_seats, 0}, {0, n_seats, 0}};
	struct sembuf boarding[1] = {{2, -1, 0}};

	printf ("Captain opened doors\n\n");

	for (int i = 0; i < n_rides; i++)
	{
		printf ("Start boarding\n");
		semop (sems_id, boarding, 1);
        sleep(1);
		semop (sems_id, begin, 2);
        sleep(1);
		semop (sems_id, end, 2);

		printf ("\n");
	}

	semctl (sems_id, 0, IPC_RMID, 0);
	printf ("captain: go to sleep\n");

	return 0;
}


int passenger (int sems_id, int passenger_no, int n_seats)
{
	struct sembuf board[4]   = {{0, -1, 0}, {1, -1, 0}, {2, 0, 0}, {1, 1, 0}};
	struct sembuf unboard[3] = {{0, 1, 0},  {1, -1, 0}, {1, 1, 0}};

	printf ("passenger%d: ready for carriage\n", passenger_no);

	while (1)
	{
		if ((semop (sems_id, board, 4) == -1) && ((errno == EIDRM) || (errno == EINVAL)))
		{
			printf ("passenger%d:  leaves bay\n", passenger_no);
			return 0;
		}
        printf ("passenger%d: on trap\n", passenger_no);
		printf ("passenger%d: on boat\n", passenger_no);

		sleep (1);

		printf ("passenger%d: ready for unboard\n", passenger_no);

		semop (sems_id, unboard, 3);

		printf ("passenger%d:  off to the ground\n", passenger_no);
	}
}