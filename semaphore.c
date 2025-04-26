#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int create_semaphore()
{
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1)
    {
        perror("semget failed");
        exit(1);
    }
    if (semctl(semid, 0, SETVAL, 1) == -1)
    {
        perror("semctl SETVAL failed");
        exit(1);
    }
    return semid;
}

void lock_semaphore(int semid)
{
    struct sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1)
    {
        perror("semop lock failed");
        exit(1);
    }
}

void unlock_semaphore(int semid)
{
    struct sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1)
    {
        perror("semop unlock failed");
        exit(1);
    }
}

void remove_semaphore(int semid)
{
    if (semctl(semid, 0, IPC_RMID) == -1)
    {
        perror("semctl remove failed");
    }
}
