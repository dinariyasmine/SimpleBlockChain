#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "utils.h"
#include "merkle.h"

void calculate_block_hash(Block* block) {
    // First calculate the Merkle root
    get_merkle_root(block);
    
    // Concatenate the block data
    char buffer[1024];
    sprintf(buffer, "%d%ld%s%s", 
            block->index, 
            block->timestamp, 
            block->previous_hash, 
            block->merkle_root);
    
    // Calculate the block hash
    sha256_hash(buffer, block->current_hash);
}

Block* create_block(int index, const char* previous_hash) {
    Block* block = (Block*)malloc(sizeof(Block));
    if (block == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    
    block->index = index;
    block->timestamp = time(NULL);
    block->transaction_count = 0;
    
    strncpy(block->previous_hash, previous_hash, 64);
    block->previous_hash[64] = '\0';
    
    memset(block->current_hash, 0, 65);
    memset(block->merkle_root, 0, 65);
    
    return block;
}

Block* add_transaction(Block* block, const char* input) {
    if (block->transaction_count >= 10) {
        printf("Transaction limit reached.\n");
        return block;
    }

    Transaction tx;
    if (!parse_transaction(input, &tx)) {
        printf("Invalid transaction format. Expected: 'A sends 50 DA to B'\n");
        return block;
    }

    block->transactions[block->transaction_count++] = tx;
    calculate_block_hash(block);
    return block;
}

void display_block(Block* block) {
    char time_str[30];
    struct tm *timeinfo;
    timeinfo = localtime(&block->timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("\n=== BLOCK #%d ===\n", block->index);
    printf("Timestamp: %s\n", time_str);
    printf("Previous Hash: %s\n", block->previous_hash);
    printf("Merkle Root: %s\n", block->merkle_root);
    printf("Current Hash: %s\n", block->current_hash);
    printf("Transactions (%d):\n", block->transaction_count);
    for (int i = 0; i < block->transaction_count; i++) {
        Transaction* tx = &block->transactions[i];
        printf("  %d. %s sends %d DA to %s\n", i+1, tx->sender, tx->amount, tx->receiver);
    }
    
    printf("================\n");
}

Block* deep_copy_block(Block* original) {
    Block* copy = (Block*)malloc(sizeof(Block));
    if (copy == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    
    memcpy(copy, original, sizeof(Block));
    return copy;
}
