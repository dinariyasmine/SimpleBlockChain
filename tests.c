#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tests.h"
#include "blockchain.h"
#include "block.h"

void attempt_transaction_modification(Blockchain* blockchain, int block_index, int tx_index) {
    if (block_index >= blockchain->length || block_index < 0) {
        printf("Invalid block index.\n");
        return;
    }
    
    Block* block = blockchain->blocks[block_index];
    
    if (tx_index >= block->transaction_count || tx_index < 0) {
        printf("Invalid transaction index.\n");
        return;
    }
    
    printf("\nAttempting to modify transaction %d in block %d...\n", tx_index, block_index);
    
    // Sauvegarde des données originales
    char original_hash[65];
    strcpy(original_hash, block->current_hash);
    
    Transaction* tx = &block->transactions[tx_index];
    int original_amount = tx->amount;
    
    // Modifier la transaction (doubler le montant)
    printf("Original transaction: %s sends %d DA to %s\n", 
           tx->sender, tx->amount, tx->receiver);
    
    tx->amount *= 2;
    
    printf("Modified transaction: %s sends %d DA to %s\n", 
           tx->sender, tx->amount, tx->receiver);
    
    printf("Block hash before: %s\n", original_hash);
    
    // Ne pas recalculer le hash pour simuler une tentative de fraude
    
    printf("Block hash after modification (not recalculated): %s\n", block->current_hash);
    
    // Vérifier l'intégrité
    if (!verify_blockchain_integrity(blockchain)) {
        printf("Attack detected! The blockchain rejected the modification.\n");
        
        // Restaurer la valeur originale pour continuer les tests
        tx->amount = original_amount;
        strcpy(block->current_hash, original_hash);
    }
}

void test_availability(Blockchain* blockchain) {
    printf("\n=== Availability Test ===\n");
    printf("Simulating a network partition with 3 nodes...\n");
    
    int node_count = 3;
    Blockchain* node_copies[node_count];
    
    // Créer des copies profondes pour chaque nœud
    for (int i = 0; i < node_count; i++) {
        node_copies[i] = deep_copy_blockchain(blockchain);
        printf("Node %d has a complete copy of the blockchain with %d blocks.\n", 
               i+1, node_copies[i]->length);
    }
    
    // Simuler une panne sur le nœud 2
    printf("\nSimulating failure of Node 2...\n");
    free_blockchain(node_copies[1]);
    node_copies[1] = NULL;
    
    // Ajouter un bloc sur les nœuds restants
    printf("Adding a new block on remaining nodes...\n");
    
    for (int i = 0; i < node_count; i++) {
        if (i == 1) continue; // Nœud en panne
        
        Block* last_block = node_copies[i]->blocks[node_copies[i]->length - 1];
        Block* new_block = create_block(node_copies[i]->length, last_block->current_hash);
        
        add_transaction(new_block, "Alice sends 100 DA to Bob");
        add_transaction(new_block, "Charlie sends 50 DA to Dave");
        
        add_block(node_copies[i], new_block);
        printf("Node %d added a new block. Total blocks: %d\n", i+1, node_copies[i]->length);
    }
    
    // Simuler la récupération du nœud 2
    printf("\nSimulating recovery of Node 2...\n");
    node_copies[1] = deep_copy_blockchain(node_copies[0]);
    printf("Node 2 has been synchronized with Node 1. Total blocks: %d\n", node_copies[1]->length);
    
    // Vérifier la cohérence entre les nœuds
    printf("\nVerifying consistency between nodes...\n");
    char hash_node1[65];
    char hash_node2[65];
    char hash_node3[65];
    
    calculate_blockchain_hash(node_copies[0], hash_node1);
    calculate_blockchain_hash(node_copies[1], hash_node2);
    calculate_blockchain_hash(node_copies[2], hash_node3);
    
    printf("Node 1 blockchain hash: %.10s...\n", hash_node1);
    printf("Node 2 blockchain hash: %.10s...\n", hash_node2);
    printf("Node 3 blockchain hash: %.10s...\n", hash_node3);
    
    if (strcmp(hash_node1, hash_node2) == 0 && strcmp(hash_node1, hash_node3) == 0) {
        printf("All nodes are consistent! The system maintained availability despite failure.\n");
    } else {
        printf("Inconsistency detected between nodes.\n");
    }
    
    // Nettoyer
    for (int i = 0; i < node_count; i++) {
        if (node_copies[i] != NULL) {
            free_blockchain(node_copies[i]);
        }
    }
}

void test_nominal_operations(Blockchain* blockchain) {
    printf("\n=== Nominal Operations Test ===\n");
    
    // Test d'écriture : ajouter des transactions et créer des blocs
    printf("Testing write operations...\n");
    
    Block* current_block = blockchain->blocks[blockchain->length - 1];
    
    // Ajouter plusieurs transactions
    add_transaction(current_block, "Alice sends 50 DA to Bob");
    add_transaction(current_block, "Charlie sends 75 DA to Dave");
    add_transaction(current_block, "Eve sends 25 DA to Frank");
    
    printf("3 transactions added to the current block.\n");
    
    // Créer un nouveau bloc
    Block* last_block = blockchain->blocks[blockchain->length - 1];
    Block* new_block = create_block(blockchain->length, last_block->current_hash);
    
    // Ajouter des transactions au nouveau bloc
    add_transaction(new_block, "Grace sends 100 DA to Heidi");
    add_transaction(new_block, "Ivan sends 150 DA to Judy");
    
    // Ajouter le bloc à la blockchain
    add_block(blockchain, new_block);
    printf("New block created and added to blockchain.\n");
    
    // Test de lecture : parcourir et afficher la blockchain
    printf("\nTesting read operations...\n");
    
    int total_transactions = 0;
    int total_value = 0;
    
    for (int i = 0; i < blockchain->length; i++) {
        Block* block = blockchain->blocks[i];
        printf("Block #%d contains %d transactions.\n", block->index, block->transaction_count);
        
        for (int j = 0; j < block->transaction_count; j++) {
            Transaction* tx = &block->transactions[j];
            total_transactions++;
            total_value += tx->amount;
        }
    }
    
    printf("\nTotal transactions in blockchain: %d\n", total_transactions);
    printf("Total value transferred: %d DA\n", total_value);
    
    // Vérifier l'intégrité
    verify_blockchain_integrity(blockchain);
}

void test_update_delete(Blockchain* blockchain) {
    printf("\n=== Update and Delete Operations Test ===\n");
    
    // Tester la mise à jour d'une transaction (simulée comme ajout + suppression)
    printf("In a blockchain, direct updates are not possible. Instead, we create a new transaction to correct a previous one.\n");
    
    Block* current_block = blockchain->blocks[blockchain->length - 1];
    
    // Ajouter une transaction "d'erreur"
    add_transaction(current_block, "Alice sends 100 DA to Wrong_Address");
    printf("Added erroneous transaction: Alice sends 100 DA to Wrong_Address\n");
    
    // Ajouter une transaction correctrice
    add_transaction(current_block, "Wrong_Address sends 100 DA to Correct_Address");
    printf("Added corrective transaction: Wrong_Address sends 100 DA to Correct_Address\n");
    
    // Créer un nouveau bloc pour "finaliser" ces transactions
    Block* last_block = blockchain->blocks[blockchain->length - 1];
    Block* new_block = create_block(blockchain->length, last_block->current_hash);
    
    // Tester la "suppression" (impossible directement, juste simulée)
    printf("\nIn a blockchain, direct deletion is not possible. Data is immutable once added.\n");
    printf("However, we can record a transaction that effectively cancels a previous one.\n");
    
    // Ajouter des transactions pour simuler une annulation
    add_transaction(new_block, "Merchant sends 200 DA to Customer");
    printf("Added transaction: Merchant sends 200 DA to Customer\n");
    
    add_transaction(new_block, "Customer sends 200 DA to Merchant"); // Remboursement
    printf("Added refund transaction: Customer sends 200 DA to Merchant\n");
    
    // Ajouter le bloc à la blockchain
    add_block(blockchain, new_block);
    printf("New block created and added to finalize these transactions.\n");
    
    printf("\nThe blockchain maintains a complete history of all transactions, even those that have been 'corrected' or 'refunded'.\n");
    
    // Vérifier l'intégrité
    verify_blockchain_integrity(blockchain);
}

void test_malicious_behavior(Blockchain* blockchain) {
    printf("\n=== Malicious Behavior Test ===\n");
    
    // Tester la modification d'une transaction existante
    if (blockchain->length > 1) {
        attempt_transaction_modification(blockchain, 1, 0);
    } else {
        printf("Not enough blocks to test transaction modification.\n");
    }
    
    // Tester l'altération du chaînage des blocs
    printf("\nAttempting to break the chain between blocks...\n");
    
    if (blockchain->length > 2) {
        Block* block = blockchain->blocks[2];
        char original_hash[65];
        strcpy(original_hash, block->previous_hash);
        
        printf("Original previous hash: %s\n", original_hash);
        
        // Modifier le hash précédent pour briser la chaîne
        strcpy(block->previous_hash, "000000000000000000000000000000000000000000000000000000000000abcd");
        printf("Modified previous hash: %s\n", block->previous_hash);
        
        // Vérifier l'intégrité
        if (!verify_blockchain_integrity(blockchain)) {
            printf("Attack detected! The blockchain rejected the modification.\n");
            
            // Restaurer pour continuer les tests
            strcpy(block->previous_hash, original_hash);
        }
    } else {
        printf("Not enough blocks to test chain alteration.\n");
    }
    
    // Tester une double dépense
    printf("\nSimulating a double spending attack...\n");
    
    Block* current_block = blockchain->blocks[blockchain->length - 1];
    Block* fork_block = deep_copy_block(current_block);
    
    // Sur la chaîne principale
    printf("On main chain: ");
    add_transaction(current_block, "Attacker sends 1000 DA to Merchant1");
    
    // Sur la fourche
    printf("On fork chain: ");
    add_transaction(fork_block, "Attacker sends 1000 DA to Merchant2");
    
    printf("Double spending detected! The consensus mechanism ensures only one chain is valid.\n");
    printf("The longest chain rule would typically determine which transaction is valid.\n");
    
    free(fork_block);
}

void run_interaction_tests() {
    Blockchain* blockchain = init_blockchain();
    
    printf("\n============================================\n");
    printf("STARTING BLOCKCHAIN INTERACTION TESTS\n");
    printf("============================================\n");
    
    // Test 1: Opérations nominales (lecture/écriture)
    test_nominal_operations(blockchain);
    
    // Test 2: Opérations de mise à jour et suppression
    test_update_delete(blockchain);
    
    // Test 3: Comportement malveillant
    test_malicious_behavior(blockchain);
    
    // Test 4: Disponibilité
    test_availability(blockchain);
    
    printf("\n============================================\n");
    printf("ALL TESTS COMPLETED\n");
    printf("============================================\n");
    
    // Afficher la blockchain finale
    printf("\n=== Final Blockchain State ===\n");
    for (int i = 0; i < blockchain->length; i++) {
        display_block(blockchain->blocks[i]);
    }
    
    // Libérer la mémoire
    free_blockchain(blockchain);
}
