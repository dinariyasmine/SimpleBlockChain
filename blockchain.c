#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Structure of a Merkle tree node
typedef struct MerkleNode {
    char hash[65];
    // Children
    struct MerkleNode* left;
    struct MerkleNode* right;
} MerkleNode;

// Structure of a block
typedef struct Block {
    int index;                     
    time_t timestamp;              
    char transaction[10][256];     
    int transaction_count;         
    char previous_hash[65];       
    char current_hash[65];
    char merkle_root[65];        
} Block;

// Utility function
void sha256_hash(const char* input, char output[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);
    
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

// To create a Merkle tree node
MerkleNode* create_merkle_node(const char* data) {
    MerkleNode* node = (MerkleNode*)malloc(sizeof(MerkleNode));
    if (node == NULL) {
        printf("Memory error\n");
        exit(1);
    }
    
    sha256_hash(data, node->hash);
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/* Each transaction becomes a leaf node, then we move up to
create intermediate nodes and then their root 
(bottom up approach)=> each block will be represented by a Merkle tree */
MerkleNode* build_merkle_tree(char transactions[][256], int count) {
    if (count == 0) return NULL;
    
    MerkleNode** leaf_nodes = (MerkleNode**)malloc(count * sizeof(MerkleNode*));
    for (int i = 0; i < count; i++) {
        leaf_nodes[i] = create_merkle_node(transactions[i]);
    }
    
    int n = count;
    if (n % 2 == 1) {
        leaf_nodes[n] = create_merkle_node(transactions[n-1]);
        n++;
    }
    
    int level_size = n;
    MerkleNode** current_level = leaf_nodes;
    
    while (level_size > 1) {
        int next_level_size = (level_size + 1) / 2;
        MerkleNode** next_level = (MerkleNode**)malloc(next_level_size * sizeof(MerkleNode*));
        
        for (int i = 0; i < level_size; i += 2) {
            MerkleNode* parent = (MerkleNode*)malloc(sizeof(MerkleNode));
            parent->left = current_level[i];
            
            if (i + 1 < level_size) {
                parent->right = current_level[i + 1];
            } else {
                parent->right = current_level[i];
            }
            
            char combined[130];
            sprintf(combined, "%s%s", parent->left->hash, parent->right->hash);
            sha256_hash(combined, parent->hash);
            
            next_level[i/2] = parent;
        }
        
        if (current_level != leaf_nodes) {
            free(current_level);
        }
        
        current_level = next_level;
        level_size = next_level_size;
    }
    
    MerkleNode* root = current_level[0];
    free(current_level);
    free(leaf_nodes);
    
    return root;
}

void get_merkle_root(Block* block) {
    if (block->transaction_count == 0) {
        strcpy(block->merkle_root, "0000000000000000000000000000000000000000000000000000000000000000");
        return;
    }
    
    MerkleNode* root = build_merkle_tree(block->transaction, block->transaction_count);
    strcpy(block->merkle_root, root->hash);
    
    // Free the Merkle tree (recursive function to implement later)
    // free_merkle_tree(root);
}

// Function to calculate the block hash
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

// Function to add a transaction to a block
Block* add_transaction(Block* block, const char* transaction) {
    if (block->transaction_count < 10) {
        strncpy(block->transaction[block->transaction_count], transaction, 255);
        block->transaction[block->transaction_count][255] = '\0';
        block->transaction_count++;
        
        // Recalculate the hash after adding a transaction
        calculate_block_hash(block);
    } else {
        printf("Transaction limit reached.\n");
    }
    return block;
}

// Function to display a block
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
        printf("  %d. %s\n", i+1, block->transaction[i]);
    }
    printf("================\n");
}

// Structure to represent the blockchain
typedef struct {
    Block** blocks;
    int length;
    int capacity;
} Blockchain;

// Function to initialize a blockchain
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
    
    // Create the genesis block
    Block* genesis = create_block(0, "0000000000000000000000000000000000000000000000000000000000000000");
    add_transaction(genesis, "Genesis Block");
    
    // Add the genesis block to the blockchain
    blockchain->blocks[blockchain->length++] = genesis;
    
    return blockchain;
}

// Function to add a block to the blockchain
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

int main() {

    Blockchain* blockchain = init_blockchain();
    
    int running = 1;
    char input[256];

    printf("Welcome to the Simple Blockchain \n");
    
    while (running) {
        // eventlistener
        printf("\nEnter a transaction (exemple: 'Aicha sends 50 da to Yasmine'), or type 'newblock' to create a new block, or 'exit' to quit:\n> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; 
        
        if (strcmp(input, "exit") == 0) {
            running = 0;
        } else if (strcmp(input, "newblock") == 0) {
            Block* last_block = blockchain->blocks[blockchain->length - 1];
            Block* new_block = create_block(blockchain->length, last_block->current_hash);
            add_block(blockchain, new_block);
            printf("New block created\n");
        } else {
            Block* current_block = blockchain->blocks[blockchain->length - 1];
            add_transaction(current_block, input);
            printf("Transaction added\n");
        }
    }
    
    // Dispkay blockchain
    printf("\n=== Blockchain Preview ===\n");
    for (int i = 0; i < blockchain->length; i++) {
        display_block(blockchain->blocks[i]);
    }

    return 0;
}

