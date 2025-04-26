# Concurrent Banking System

## Overview

This project is developed for **Operating Systems Assignment 2**.  
It implements a **Concurrent Banking System** using **Processes**, **Shared Memory**, **Semaphores**, and **Synchronization Techniques**.

Multiple transactions (Deposit, Withdraw, Transfer) are processed concurrently without causing race conditions or deadlocks.

---

## Project Structure

```
concurrent_banking/
├── main.c
├── transaction.c
├── transaction.h
├── semaphore.c
├── semaphore.h
├── shared.h
├── transactions.txt
├── Makefile
└── README.md
```

---

## How It Works

- **Accounts** are initialized in shared memory with predefined balances.
- **Transactions** are loaded from a file (`transactions.txt`).
- For **each transaction**:
  - A new **child process** is forked.
  - Access to shared memory is **protected by semaphores**.
  - **Deadlocks are prevented** by locking accounts in a fixed order (lower account ID first).
  - If a transaction **fails** (e.g., insufficient funds), it is **retried once**.
- After all transactions are completed:
  - Final account balances are printed.
  - A detailed transaction log is displayed.

---

## How to Build and Run

1. **Extract** the project folder.

2. Open a **terminal** and navigate to the project directory:
   ```bash
   cd concurrent_banking
   ```

3. **Build** the project using `make`:
   ```bash
   make
   ```

4. **Run** the program:
   ```bash
   ./main
   ```

5. (Optional) To **clean** compiled files:
   ```bash
   make clean
   ```

---

## Sample Output

```plaintext
Accounts initialized successfully.

Processing Transaction ID: 0
Processing Transaction ID: 1
Processing Transaction ID: 2
...

Final Account Balances:
Account 0: 630
Account 1: 510
Account 2: 5
Account 3: 1055
Account 4: 610

Transaction Log:
Transaction 0: Deposit 100 to Account 0 (Success)
Transaction 1: Withdraw 50 from Account 1 (Success)
Transaction 2: Transfer 615 from Account 2 to Account 3 (Failed)
Transaction 3: Deposit 200 to Account 4 (Success)
Transaction 4: Transfer 30 from Account 1 to Account 0 (Success)
Transaction 5: Withdraw 60 from Account 3 (Success)
Transaction 6: Deposit 120 to Account 2 (Success)
Transaction 7: Transfer 90 from Account 4 to Account 1 (Success)
Transaction 2 (Retry): Transfer 615 from Account 2 to Account 3 (Success)
```

---

## Key Concepts Used

- **Process Management** (`fork`, `waitpid`)
- **Shared Memory Communication** (`shmget`, `shmat`, `shmdt`, `shmctl`)
- **Synchronization** with **Semaphores** (`semget`, `semop`, `semctl`)
- **Deadlock Prevention** (account locking order)
- **Error Handling and Recovery** (retry failed transactions)

---

## Notes

- Make sure you have **GCC** compiler installed.
- Project is tested on **Ubuntu 22.04 LTS** and **WSL2 (Ubuntu)**.
- Follow proper **build and run steps** to ensure correct execution.

---

> Project prepared by **Ahmet Yıldız** for **Operating Systems** course, Concurrent Banking System Assignment.
