#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "block.h"

typedef struct {
    Block** blocks;
    int length;
    int capacity;
} Blockchain;

typedef struct {
    int id;
    Blockchain* replica;
} PeerNode;

// Blockchain operations
Blockchain* init_blockchain();
void add_block(Blockchain* blockchain, Block* block);
Blockchain* deep_copy_blockchain(Blockchain* original);
void free_blockchain(Blockchain* blockchain);
void calculate_blockchain_hash(Blockchain* blockchain, char* output);
int verify_blockchain_integrity(Blockchain* blockchain);

#endif
