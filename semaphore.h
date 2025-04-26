#ifndef SEMAPHORE_H
#define SEMAPHORE_H

int create_semaphore();
void lock_semaphore(int semid);
void unlock_semaphore(int semid);
void remove_semaphore(int semid);

#endif // SEMAPHORE_H
