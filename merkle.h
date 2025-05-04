#ifndef MERKLE_H
#define MERKLE_H

#include "block.h"

typedef struct MerkleNode {
    char hash[65];
    struct MerkleNode* left;
    struct MerkleNode* right;
} MerkleNode;

MerkleNode* create_merkle_node(const char* data);
MerkleNode* build_merkle_tree(char transactions[][256], int count);
void free_merkle_tree(MerkleNode* node);
void get_merkle_root(Block* block);

#endif
