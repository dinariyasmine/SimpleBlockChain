// ui.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "blockchain.h"
#include "block.h"

void display_menu() {
    printf("\n===== BLOCKCHAIN DEMO =====\n");
    printf("1. Add transaction to current block\n");
    printf("2. Create new block\n");
    printf("3. View blockchain\n");
    printf("4. Verify blockchain integrity\n");
    printf("5. Simulate attack\n");
    printf("6. Run automated tests\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

void handle_add_transaction(Blockchain* blockchain) {
    char transaction[256];
    
    printf("\nEnter transaction (format: 'Sender sends Amount DA to Receiver'): ");
    fflush(stdin);
    fgets(transaction, sizeof(transaction), stdin);
    
    // Remove newline character
    transaction[strcspn(transaction, "\n")] = 0;
    
    Block* current_block = blockchain->blocks[blockchain->length - 1];
    add_transaction(current_block, transaction);
    
    printf("Transaction added to block #%d\n", current_block->index);
}

void handle_create_block(Blockchain* blockchain) {
    Block* last_block = blockchain->blocks[blockchain->length - 1];
    
    if (last_block->transaction_count == 0) {
        printf("\nCannot create empty block. Add at least one transaction first.\n");
        return;
    }
    
    Block* new_block = create_block(blockchain->length, last_block->current_hash);
    add_block(blockchain, new_block);
    
    printf("\nNew block #%d created and added to the blockchain.\n", new_block->index);
}

void handle_view_blockchain(Blockchain* blockchain) {
    printf("\n===== BLOCKCHAIN CONTENTS =====\n");
    
    for (int i = 0; i < blockchain->length; i++) {
        display_block(blockchain->blocks[i]);
    }
}

void handle_verify_integrity(Blockchain* blockchain) {
    printf("\nVerifying blockchain integrity...\n");
    verify_blockchain_integrity(blockchain);
}

void handle_simulate_attack(Blockchain* blockchain) {
    int block_index, tx_index;
    
    printf("\n=== EDUCATIONAL ATTACK SIMULATION ===\n");
    printf("This demonstrates why blockchains are secure.\n\n");
    
    printf("Enter block index to attack: ");
    scanf("%d", &block_index);
    
    if (block_index <= 0 || block_index >= blockchain->length) {
        printf("Invalid block index. Try again.\n");
        return;
    }
    
    printf("Enter transaction index within block: ");
    scanf("%d", &tx_index);
    
    attempt_transaction_modification(blockchain, block_index, tx_index);
}

void run_ui(Blockchain* blockchain) {
    int choice;
    
    do {
        display_menu();
        scanf("%d", &choice);
        getchar(); // Consume newline
        
        switch (choice) {
            case 1:
                handle_add_transaction(blockchain);
                break;
            case 2:
                handle_create_block(blockchain);
                break;
            case 3:
                handle_view_blockchain(blockchain);
                break;
            case 4:
                handle_verify_integrity(blockchain);
                break;
            case 5:
                handle_simulate_attack(blockchain);
                break;
            case 6:
                run_interaction_tests();
                break;
            case 0:
                printf("SimpleBlockChain session terminated successfully.\n");

                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}
// Add to ui.c
void visualize_blockchain(Blockchain* blockchain) {
    printf("\n=== BLOCKCHAIN VISUALIZATION ===\n\n");
    
    for (int i = 0; i < blockchain->length; i++) {
        Block* block = blockchain->blocks[i];
        
        // Print block header
        printf("┌─────────────────────┐\n");
        printf("│ BLOCK #%-12d │\n", block->index);
        printf("├─────────────────────┤\n");
        printf("│ TX Count: %-9d │\n", block->transaction_count);
        printf("│ Hash: %.10s...     │\n", block->current_hash);
        printf("└─────────────────────┘\n");
        
        if (i < blockchain->length - 1) {
            printf("          ↓\n");  // Chain link
        }
    }
}

