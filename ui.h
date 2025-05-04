// ui.h
#ifndef UI_H
#define UI_H

#include "blockchain.h"

void run_ui(Blockchain* blockchain);
void display_menu();
void handle_add_transaction(Blockchain* blockchain);
void handle_create_block(Blockchain* blockchain);
void handle_view_blockchain(Blockchain* blockchain);
void handle_verify_integrity(Blockchain* blockchain);
void handle_simulate_attack(Blockchain* blockchain);

#endif
