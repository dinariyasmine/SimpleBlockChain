#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "utils.h"

void transaction_to_string(Transaction* tx, char* output, size_t size) {
    snprintf(output, size, "%s sends %d DA to %s", tx->sender, tx->amount, tx->receiver);
}

int parse_transaction(const char* input, Transaction* tx) {
    char temp[256];
    strncpy(temp, input, 255);
    temp[255] = '\0';

    // Make lowercase copy for parsing
    char* lower = to_lowercase_copy(temp);
    if (!lower) return 0;

    char sender[64], receiver[64];
    int amount;

    int success = sscanf(lower, "%63s sends %d da to %63s", sender, &amount, receiver);
    free(lower);

    if (success == 3 && amount > 0) {
        // Copy original names from input string to preserve casing
        sscanf(input, "%63s sends %d DA to %63s", tx->sender, &tx->amount, tx->receiver);
        return 1;
    }

    return 0;
}

int validate_transaction(const char* transaction) {
    return (strstr(transaction, "sends") && strstr(transaction, "DA"));
}
