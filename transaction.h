#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "shared.h"

// Transaction işlemleri
void load_transactions(BankData *data, const char *filename);
int process_deposit(BankData *data, int account_id, int amount);
int process_withdraw(BankData *data, int account_id, int amount);
int process_transfer(BankData *data, int from_account, int to_account, int amount);
void process_transaction(BankData *data, Transaction *transaction, int semid);

#endif // TRANSACTION_H
