#include <pthread.h>
#include "bank.h"
#include "string_parser.h"
#include "account.h"
#include <stdlib.h> // for malloc
#include <stdio.h> // i/o functions
#include <string.h> // for strcmp
#include <math.h> // for rounding


account **acc;
int num_of_acc;
int num_of_transaction = 0;
int num_of_updates = 0;

int main(int argc, char *argv[]) {
    // file initiaization
    char *line = NULL;
    size_t length = 0;

    // open input file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("couldn't open file");
        return EXIT_FAILURE;
    }
    // retrieve number of accounts
    command_line line_parsed;
    getline(&line, &length, file);
    line_parsed = str_filler(line, " ");
    num_of_acc = atoi(line_parsed.command_list[0]);
    free_command_line(&line_parsed);
    free(line);
    line = NULL;

    // create array of account class objects
    populate_acc(&acc, num_of_acc, file);

    while (getline(&line, &length, file) != -1) {
        process_transaction(line);
        free(line);
        line = NULL;
    }
    free(line);

    printf("Number of rewards applied: %d\n", num_of_updates);

    //close file
    fclose(file);

    // print out account balances
    file = fopen("output/output.txt", "a");

    for (int i = 0; i < num_of_acc; i++) {
        fprintf(file, "%d balance:\t%.2f\n\n", i, acc[i]->balance);
    }
    fclose(file);

    // freeing account array
    for (int i = 0; i < num_of_acc; i++) {
        free(acc[i]);
    }
    free(acc);
}

// populate account structs
void populate_acc(account ***acc, int num_of_acc, FILE *file) {
    // array of account structs
    *acc = (account **)malloc(sizeof(account *) * num_of_acc);
    for (int i = 0; i < num_of_acc; i++) {
        (*acc)[i] = (account *)malloc(sizeof(account));
    }
    // reading file
    char *line = NULL;
    size_t length = 0;
    command_line line_parsed;

    // for indexing account were populating in acc
    int acc_index = 0;
    // for mapping account info to struct
    int line_count = 0;
    // for mapping input account info line to struct
    int acc_map_i = 0;

    // loop over each input line
    // populating the account structs
    while (1) { 
        getline(&line, &length, file);
        // cleans up line
        line_parsed = str_filler(line, " ");
        // update the account index
        if (strcmp(line_parsed.command_list[0], "index") == 0) {
            acc_index = atoi(line_parsed.command_list[1]);
            acc_map_i = 0; // reset for next indexed account
            line_count = 0;
        }
        if (line_count == 1){
            strcpy((*acc)[acc_index]->account_number, line_parsed.command_list[0]);
        }
        else if (line_count == 2){
            strcpy((*acc)[acc_index]->password, line_parsed.command_list[0]);
        }
        else if (line_count == 3){
            (*acc)[acc_index]->balance = atof(line_parsed.command_list[0]);
        }
        else if (line_count == 4){
            (*acc)[acc_index]->reward_rate = atof(line_parsed.command_list[0]);
        }
        (*acc)[acc_index]->transaction_tracker = 0;

        acc_map_i++;
        line_count++;

        free_command_line(&line_parsed);
        free(line);
        line = NULL;
        if (acc_index == (num_of_acc-1) && line_count == 5) {
            break;
        }
    }
}

void *process_transaction(void *arg) {
    command_line line_parsed;
    line_parsed = str_filler(arg, " ");
    char acc_num[17];
    char pass[9];
    strcpy(acc_num, line_parsed.command_list[1]);
    strcpy(pass, line_parsed.command_list[2]);

    // find which account we're accessing
    int src_acc_exists = 0;
    int dst_acc_exists = 0; // for transfers
    int acc_src;
    int acc_dst;
    for (int i = 0; i < num_of_acc; i++) {
        // grab index of src account
        if (strcmp(acc[i]->account_number, acc_num) == 0) {
            if (strcmp(acc[i]->password, pass) == 0) {
                src_acc_exists = 1;
                acc_src = i;
            }
        }
        // if the op is transfer, grab dest account index
        if (strcmp(line_parsed.command_list[0], "T") == 0) {
            if (strcmp(acc[i]->account_number, line_parsed.command_list[3]) == 0) {
                dst_acc_exists = 1;
                acc_dst = i;
            }
        }
    }
    // if account password pair doesn't exist return
    if (src_acc_exists == 0) {
        free_command_line(&line_parsed);
        return NULL;
    }
    // checking is dst acount exists for transfer
    if (strcmp(line_parsed.command_list[0], "T") == 0 && dst_acc_exists == 0) {
        free_command_line(&line_parsed);
        return NULL;
    }

    // account operations
    // transfering funds
    double transfer_amount;
    char *endptr;
    if (strcmp(line_parsed.command_list[0], "T") == 0) {
        transfer_amount = strtod(line_parsed.command_list[4], &endptr); 
        // printf("acc_src start: %f\n", acc[acc_src]->balance);
        // printf("acc_dst start: %f\n", acc[acc_dst]->balance);
        // printf("transfering %f\n", transfer_amount);
        acc[acc_src]->balance -= transfer_amount;
        acc[acc_dst]->balance += transfer_amount;
        // printf("acc_src end: %f\n", acc[acc_src]->balance);
        // printf("acc_dst end: %f\n\n", acc[acc_dst]->balance);
        acc[acc_src]->transaction_tracker += strtod(line_parsed.command_list[4], &endptr);
        num_of_transaction += 1;
    } // withdraw
    else if (strcmp(line_parsed.command_list[0], "W") == 0) {
        transfer_amount = strtod(line_parsed.command_list[3], NULL);
        acc[acc_src]->balance -= transfer_amount;
        acc[acc_src]->transaction_tracker += strtod(line_parsed.command_list[3], &endptr);
        num_of_transaction += 1;
    } // deposit
    else if (strcmp(line_parsed.command_list[0], "D") == 0) {
        transfer_amount = strtod(line_parsed.command_list[3], NULL);
        acc[acc_src]->balance += transfer_amount;
        acc[acc_src]->transaction_tracker += strtod(line_parsed.command_list[3], &endptr);
        num_of_transaction += 1;
    } // check
    else if (strcmp(line_parsed.command_list[0], "C") == 0) {
        // nothing to put here ...
    }

    if (num_of_transaction == 5000) {
        update_balance();
    }
    free_command_line(&line_parsed);
    return NULL;
}

void *update_balance() {
    printf("bank applying rewards...\n");
    for (int i = 0; i < num_of_acc; i++) {
        acc[i]->balance += acc[i]->reward_rate * acc[i]->transaction_tracker;
        acc[i]->transaction_tracker = 0.0;
    }
    num_of_transaction = 0;
    num_of_updates += 1;
    return &num_of_updates;
}