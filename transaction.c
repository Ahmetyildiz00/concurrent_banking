#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#include "semaphore.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

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

    for (int i = count; i < MAX_TRANSACTIONS; i++)
    {
        data->transactions[i].transaction_id = -1;
    }

    fclose(file);
    printf("%d transactions loaded.\n", count);
}

int process_deposit(BankData *data, int account_id, int amount)
{
    if (amount <= 0)
        return 0;
    data->accounts[account_id].balance += amount;
    return 1;
}

int process_withdraw(BankData *data, int account_id, int amount)
{
    if (amount <= 0 || data->accounts[account_id].balance < amount)
        return 0;
    data->accounts[account_id].balance -= amount;
    return 1;
}

int process_transfer(BankData *data, int from_account, int to_account, int amount)
{
    if (amount <= 0 || data->accounts[from_account].balance < amount)
        return 0;
    data->accounts[from_account].balance -= amount;
    data->accounts[to_account].balance += amount;
    return 1;
}

void process_transaction(BankData *data, Transaction *transaction, int semid)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        int result = 0;

        lock_semaphore(semid);

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

        unlock_semaphore(semid);

        if (result)
            exit(0);
        else
            exit(-1);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);
            transaction->success = (exit_code == 0) ? 1 : 0;
        }
    }
    else
    {
        perror("fork failed");
    }
}
