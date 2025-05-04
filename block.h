#ifndef BLOCK_H
#define BLOCK_H

#include <time.h>
#include "transaction.h"

typedef struct Block {
    int index;                     
    time_t timestamp;              
    Transaction transactions[10];    
    int transaction_count;         
    char previous_hash[65];       
    char current_hash[65];
    char merkle_root[65];        
} Block;

// Block operations
Block* create_block(int index, const char* previous_hash);
Block* add_transaction(Block* block, const char* input);
void calculate_block_hash(Block* block);
void display_block(Block* block);
Block* deep_copy_block(Block* original);

#endif
