#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "transaction.h"
#include "semaphore.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
    int shmid;
    BankData *data;
    int semid;

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(BankData), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory
    data = (BankData *)shmat(shmid, NULL, 0);
    if ((void *)data == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    // Create semaphore
    semid = create_semaphore();

    // Initialize accounts
    int initial_balances[] = {500, 500, 500, 500, 500};

    for (int i = 0; i < 5; i++)
    {
        data->accounts[i].account_id = i;
        data->accounts[i].balance = initial_balances[i];
    }

    printf("Accounts initialized successfully.\n");

    // Load transactions
    load_transactions(data, "transactions.txt");

    // First pass: process all transactions
    int retry_indices[MAX_TRANSACTIONS];
    int retry_count = 0;

    for (int i = 0; i < MAX_TRANSACTIONS && data->transactions[i].transaction_id != -1; i++)
    {
        printf("\nProcessing Transaction ID: %d\n", data->transactions[i].transaction_id);

        process_transaction(data, &data->transactions[i], semid);

        if (data->transactions[i].success == 0)
        {
            retry_indices[retry_count++] = i;
        }
    }

    // Second pass: retry failed transactions
    for (int i = 0; i < retry_count; i++)
    {
        int idx = retry_indices[i];
        printf("\nRetrying Transaction ID: %d\n", data->transactions[idx].transaction_id);
        process_transaction(data, &data->transactions[idx], semid);
    }

    // Print final balances
    printf("\nFinal Account Balances:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("Account %d: %d\n", data->accounts[i].account_id, data->accounts[i].balance);
    }

    // Print transaction log
    printf("\nTransaction Log:\n");

    // Normal işlemler
    for (int i = 0; i < MAX_TRANSACTIONS && data->transactions[i].transaction_id != -1; i++)
    {
        printf("Transaction %d: ", data->transactions[i].transaction_id);

        switch (data->transactions[i].type)
        {
        case DEPOSIT:
            printf("Deposit %d to Account %d (%s)\n", data->transactions[i].amount, data->transactions[i].to_account, data->transactions[i].success ? "Success" : "Failed");
            break;
        case WITHDRAW:
            printf("Withdraw %d from Account %d (%s)\n", data->transactions[i].amount, data->transactions[i].from_account, data->transactions[i].success ? "Success" : "Failed");
            break;
        case TRANSFER:
            printf("Transfer %d from Account %d to Account %d (%s)\n", data->transactions[i].amount, data->transactions[i].from_account, data->transactions[i].to_account, data->transactions[i].success ? "Success" : "Failed");
            break;
        }
    }

    // Retry log
    for (int i = 0; i < retry_count; i++)
    {
        int idx = retry_indices[i];
        printf("Transaction %d (Retry): ", data->transactions[idx].transaction_id);

        switch (data->transactions[idx].type)
        {
        case DEPOSIT:
            printf("Deposit %d to Account %d (%s)\n", data->transactions[idx].amount, data->transactions[idx].to_account, data->transactions[idx].success ? "Success" : "Failed");
            break;
        case WITHDRAW:
            printf("Withdraw %d from Account %d (%s)\n", data->transactions[idx].amount, data->transactions[idx].from_account, data->transactions[idx].success ? "Success" : "Failed");
            break;
        case TRANSFER:
            printf("Transfer %d from Account %d to Account %d (%s)\n", data->transactions[idx].amount, data->transactions[idx].from_account, data->transactions[idx].to_account, data->transactions[idx].success ? "Success" : "Failed");
            break;
        }
    }

    // Remove semaphore
    remove_semaphore(semid);

    // Detach and remove shared memory
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
