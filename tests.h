#ifndef TESTS_H
#define TESTS_H

#include "blockchain.h"

void run_interaction_tests();
void test_nominal_operations(Blockchain* blockchain);
void test_update_delete(Blockchain* blockchain);
void test_malicious_behavior(Blockchain* blockchain);
void test_availability(Blockchain* blockchain);
void attempt_transaction_modification(Blockchain* blockchain, int block_index, int tx_index);

#endif
