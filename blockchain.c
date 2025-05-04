#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"
#include "utils.h"

Blockchain* init_blockchain() {
    Blockchain* blockchain = (Blockchain*)malloc(sizeof(Blockchain));
    if (blockchain == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    blockchain->capacity = 10;
    blockchain->length = 0;
    blockchain->blocks = (Block**)malloc(blockchain->capacity * sizeof(Block*));
    if (blockchain->blocks == NULL) {
        printf("Memory allocation error\n");
        free(blockchain);
        exit(1);
    }

    // Create the genesis block properly
    Block* genesis = create_block(0, "0");  // "0" for no previous hash
    Transaction genesis_tx = {"System", "Network", 0};
    genesis->transactions[genesis->transaction_count++] = genesis_tx;
    calculate_block_hash(genesis);

    blockchain->blocks[blockchain->length++] = genesis;

    return blockchain;
}

void add_block(Blockchain* blockchain, Block* block) {
    if (blockchain->length >= blockchain->capacity) {
        blockchain->capacity *= 2;
        blockchain->blocks = (Block**)realloc(blockchain->blocks, blockchain->capacity * sizeof(Block*));
        if (blockchain->blocks == NULL) {
            printf("Memory allocation error\n");
            exit(1);
        }
    }
    
    blockchain->blocks[blockchain->length++] = block;
}

Blockchain* deep_copy_blockchain(Blockchain* original) {
    Blockchain* copy = (Blockchain*)malloc(sizeof(Blockchain));
    if (copy == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    
    copy->capacity = original->capacity;
    copy->length = original->length;
    copy->blocks = (Block**)malloc(copy->capacity * sizeof(Block*));
    if (copy->blocks == NULL) {
        printf("Memory allocation error\n");
        free(copy);
        exit(1);
    }
    
    for (int i = 0; i < original->length; i++) {
        copy->blocks[i] = deep_copy_block(original->blocks[i]);
    }
    
    return copy;
}

void free_blockchain(Blockchain* blockchain) {
    if (blockchain == NULL) return;
    
    for (int i = 0; i < blockchain->length; i++) {
        free(blockchain->blocks[i]);
    }
    free(blockchain->blocks);
    free(blockchain);
}

void calculate_blockchain_hash(Blockchain* blockchain, char* output) {
    char buffer[10240] = {0};
    char temp[65];
    
    for (int i = 0; i < blockchain->length; i++) {
        strcat(buffer, blockchain->blocks[i]->current_hash);
    }
    
    sha256_hash(buffer, output);
}

int verify_blockchain_integrity(Blockchain* blockchain) {
    for (int i = 1; i < blockchain->length; i++) {
        Block* current_block = blockchain->blocks[i];
        Block* previous_block = blockchain->blocks[i-1];
        
        // Vérifier la liaison entre les blocs
        if (strcmp(current_block->previous_hash, previous_block->current_hash) != 0) {
            printf("Blockchain integrity compromised at block %d!\n", i);
            return 0;
        }
        
        // Revérifier le hash du bloc actuel
        char calculated_hash[65];
        char buffer[1024];
        
        // Recalculer le Merkle root pour vérifier les transactions
        char original_merkle_root[65];
        strcpy(original_merkle_root, current_block->merkle_root);
        get_merkle_root(current_block);
        
        if (strcmp(original_merkle_root, current_block->merkle_root) != 0) {
            printf("Merkle root mismatch in block %d! Transactions have been tampered with.\n", i);
            return 0;
        }
        
        // Recalculer le hash du bloc
        sprintf(buffer, "%d%ld%s%s", 
                current_block->index, 
                current_block->timestamp, 
                current_block->previous_hash, 
                current_block->merkle_root);
        
        sha256_hash(buffer, calculated_hash);
        
        if (strcmp(calculated_hash, current_block->current_hash) != 0) {
            printf("Hash mismatch in block %d! Block data has been tampered with.\n", i);
            return 0;
        }
    }
    
    printf("Blockchain integrity verified - all blocks are valid.\n");
    return 1;
}

void* replicate_block(void* arg) {
    PeerNode* node = (PeerNode*)arg;
    
    printf("Peer %d is replicating the blockchain...\n", node->id);

    for (int i = 0; i < node->replica->length; i++) {
        display_block(node->replica->blocks[i]);
    }

    pthread_exit(NULL);
}

int simulate_consensus(Block* block) {
    int approvals = 0;
    int peers = 3;

    for (int i = 0; i < peers; i++) {
        // simplified simulation of consensus
        if (block->index % 2 == 0) {
            approvals++;
        }
    }

    return (approvals > peers / 2); // Majority
}
