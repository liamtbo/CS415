#ifndef BANK_H_
#define BANK_H_

#include "account.h"
#include <stdio.h>

void populate_acc(account ***acc, int num_acc, FILE *file);
void *process_transaction(void *arg);
void *update_balance();

#endif