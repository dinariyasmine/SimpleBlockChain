#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkle.h"
#include "utils.h"
#include "transaction.h"

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

MerkleNode* build_merkle_tree(char transactions[][256], int count) {
    if (count == 0) return NULL;
    
    // Allouer l'espace pour les nœuds feuilles
    int n = count;
    // S'assurer d'avoir assez d'espace si nous avons un nombre impair de transactions
    if (n % 2 == 1) n++;
    
    MerkleNode** leaf_nodes = (MerkleNode**)malloc(n * sizeof(MerkleNode*));
    if (leaf_nodes == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    
    // Créer les nœuds feuilles
    for (int i = 0; i < count; i++) {
        leaf_nodes[i] = create_merkle_node(transactions[i]);
    }
    
    // Dupliquer la dernière transaction si nombre impair
    if (count % 2 == 1) {
        leaf_nodes[count] = create_merkle_node(transactions[count-1]);
    }
    
    int level_size = n;
    MerkleNode** current_level = leaf_nodes;
    
    // Construire l'arbre niveau par niveau
    while (level_size > 1) {
        int next_level_size = (level_size + 1) / 2;
        MerkleNode** next_level = (MerkleNode**)malloc(next_level_size * sizeof(MerkleNode*));
        if (next_level == NULL) {
            printf("Memory allocation error\n");
            exit(1);
        }
        
        for (int i = 0; i < level_size; i += 2) {
            MerkleNode* parent = (MerkleNode*)malloc(sizeof(MerkleNode));
            if (parent == NULL) {
                printf("Memory allocation error\n");
                exit(1);
            }
            
            parent->left = current_level[i];
            
            // Si nous sommes à la fin et qu'il n'y a pas de paire, utiliser le même nœud
            if (i + 1 < level_size) {
                parent->right = current_level[i + 1];
            } else {
                parent->right = current_level[i]; // Auto-référence, mais marquer pour éviter double free
            }
            
            char combined[130];
            sprintf(combined, "%s%s", parent->left->hash, parent->right->hash);
            sha256_hash(combined, parent->hash);
            
            next_level[i/2] = parent;
        }
        
        // Libérer le niveau actuel mais pas les nœuds eux-mêmes
        if (current_level != leaf_nodes) {
            free(current_level);
        }
        
        current_level = next_level;
        level_size = next_level_size;
    }
    
    // Stocker la racine avant de libérer le tableau
    MerkleNode* root = NULL;
    if (level_size > 0) {
        root = current_level[0];
    }
    
    // Libérer le dernier tableau de pointeurs
    free(current_level);
    free(leaf_nodes);
    
    return root;
}

void free_merkle_tree(MerkleNode* node) {
    if (node == NULL) return;
    
    // Vérifier si les nœuds enfants existent avant de les libérer
    if (node->left != NULL && node->left != node->right) {
        free_merkle_tree(node->left);
    }
    
    if (node->right != NULL && node->left != node->right) {
        free_merkle_tree(node->right);
    }
    
    free(node);
}

void get_merkle_root(Block* block) {
    if (block->transaction_count == 0) {
        strcpy(block->merkle_root, "0000000000000000000000000000000000000000000000000000000000000000");
        return;
    }
    
    char transaction_strings[10][256];
    for (int i = 0; i < block->transaction_count; i++) {
        transaction_to_string(&block->transactions[i], transaction_strings[i], sizeof(transaction_strings[i]));
    }

    MerkleNode* root = build_merkle_tree(transaction_strings, block->transaction_count);

    strcpy(block->merkle_root, root->hash);
    
    free_merkle_tree(root);
}
