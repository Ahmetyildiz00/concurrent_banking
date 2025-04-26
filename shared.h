#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100

typedef enum
{
    DEPOSIT,
    WITHDRAW,
    TRANSFER
} TransactionType;

typedef struct
{
    int account_id;
    int balance;
} Account;

typedef struct
{
    TransactionType type;
    int transaction_id;
    int from_account;
    int to_account;
    int amount;
    int success;
} Transaction;

typedef struct
{
    Account accounts[MAX_ACCOUNTS];
    Transaction transactions[MAX_TRANSACTIONS];
} BankData;

#endif
