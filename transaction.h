#ifndef TRANSACTION_H
#define TRANSACTION_H

typedef struct {
    char sender[64];
    char receiver[64];
    int amount;
} Transaction;

int parse_transaction(const char* input, Transaction* tx);
void transaction_to_string(Transaction* tx, char* output, size_t size);
int validate_transaction(const char* transaction);

#endif
