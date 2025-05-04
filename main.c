#include <stdio.h>
#include "blockchain.h"
#include "ui.h"

int main() {
    Blockchain* blockchain = init_blockchain();
    printf("Welcome to Blockchain Demo\n");
    printf("A genesis block has been created automatically.\n");
    
    run_ui(blockchain);
    
    // Clean up
    free_blockchain(blockchain);
    return 0;
}
