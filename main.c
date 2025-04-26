#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "transaction.c" // Include transaction functions
#include <sys/types.h>   // For pid_t, etc.
#include <sys/ipc.h>     // For IPC_PRIVATE, shmget, etc.
#include <sys/shm.h>     // For shmget, shmat
#include <unistd.h>      // For fork, getpid, etc.

extern int process_deposit(BankData *data, int account_id, int amount);
extern int process_withdraw(BankData *data, int account_id, int amount);
extern int process_transfer(BankData *data, int from_account, int to_account, int amount);
extern void load_transactions(BankData *data, const char *filename); // Add extern for load_transactions

int main()
{
    int shmid;
    BankData *data;

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(BankData), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory to process
    data = (BankData *)shmat(shmid, NULL, 0);
    if ((void *)data == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Initialize accounts with balances
    int initial_balances[] = {500, 500, 100, 1000, 400};
    int num_accounts = 5;

    for (int i = 0; i < num_accounts; i++)
    {
        data->accounts[i].account_id = i;
        data->accounts[i].balance = initial_balances[i];
    }

    printf("Accounts initialized successfully.\n");

    // Load transactions from file and process them
    load_transactions(data, "transactions.txt");

    return 0;
}
