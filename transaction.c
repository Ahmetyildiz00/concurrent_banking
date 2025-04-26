#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#include <sys/types.h> // For pid_t, etc.
#include <sys/ipc.h>   // For IPC_PRIVATE, shmget, etc.
#include <sys/shm.h>   // For shmget, shmat
#include <unistd.h>    // For fork, getpid, etc.

// Function to load transactions from a file
void load_transactions(BankData *data, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Could not open file");
        return;
    }

    char type[16];
    int id, from, to, amount;
    int count = 0;

    while (fscanf(file, "%s", type) == 1)
    {
        if (strcmp(type, "DEPOSIT") == 0)
        {
            fscanf(file, "%d %d %d", &id, &to, &amount);
            data->transactions[count++] = (Transaction){DEPOSIT, id, -1, to, amount, -1};
        }
        else if (strcmp(type, "WITHDRAW") == 0)
        {
            fscanf(file, "%d %d %d", &id, &from, &amount);
            data->transactions[count++] = (Transaction){WITHDRAW, id, from, -1, amount, -1};
        }
        else if (strcmp(type, "TRANSFER") == 0)
        {
            fscanf(file, "%d %d %d %d", &id, &from, &to, &amount);
            data->transactions[count++] = (Transaction){TRANSFER, id, from, to, amount, -1};
        }
    }

    fclose(file);
    printf("%d transactions loaded.\n", count);
}

int process_deposit(BankData *data, int account_id, int amount)
{
    if (amount <= 0)
    {
        printf("Invalid deposit amount.\n");
        return 0;
    }
    data->accounts[account_id].balance += amount;
    printf("Deposit successful: Account %d, Amount: %d\n", account_id, amount);
    return 1;
}

int process_withdraw(BankData *data, int account_id, int amount)
{
    if (amount <= 0)
    {
        printf("Invalid withdraw amount.\n");
        return 0;
    }
    if (data->accounts[account_id].balance < amount)
    {
        printf("Insufficient balance in Account %d.\n", account_id);
        return 0;
    }
    data->accounts[account_id].balance -= amount;
    printf("Withdrawal successful: Account %d, Amount: %d\n", account_id, amount);
    return 1;
}

int process_transfer(BankData *data, int from_account, int to_account, int amount)
{
    if (amount <= 0)
    {
        printf("Invalid transfer amount.\n");
        return 0;
    }
    if (data->accounts[from_account].balance < amount)
    {
        printf("Insufficient balance in Account %d for transfer.\n", from_account);
        return 0;
    }
    data->accounts[from_account].balance -= amount;
    data->accounts[to_account].balance += amount;
    printf("Transfer successful: From Account %d to Account %d, Amount: %d\n", from_account, to_account, amount);
    return 1;
}

void process_transaction(BankData *data, Transaction *transaction)
{
    pid_t pid = fork();

    if (pid == 0)
    { // Child process
        int result = 0;
        switch (transaction->type)
        {
        case DEPOSIT:
            result = process_deposit(data, transaction->to_account, transaction->amount);
            break;
        case WITHDRAW:
            result = process_withdraw(data, transaction->from_account, transaction->amount);
            break;
        case TRANSFER:
            result = process_transfer(data, transaction->from_account, transaction->to_account, transaction->amount);
            break;
        }
        exit(result); // Exit with success or failure code
    }
    else if (pid > 0)
    { // Parent process
        int status;
        wait(&status); // Wait for child process to finish
        if (WIFEXITED(status))
        {
            printf("Transaction processed successfully with result code: %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("Transaction failed.\n");
        }
    }
    else
    {
        perror("fork failed");
    }
}
