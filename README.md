
# SimpleBlockChain
![Blockchain Overview](image-1.png)

**SimpleBlockChain** is a basic blockchain implementation in C, featuring:

* **SHA-256 hashing** via OpenSSL
* **Merkle Tree** for transaction integrity
* **Block linking and validation**
* **Threaded block replication and consensus simulation**

![Merkle Tree Diagram](image.png)

---

## Features

* Create blocks containing transactions
* Build a Merkle Tree to compute a Merkle Root
* Link blocks using cryptographic hashes
* Transaction management (up to 10 per block)
* Genesis Block creation
* Blockchain integrity verification
* Simple transaction validation
* Consensus mechanism simulation
* Attack detection and prevention
* Distributed replication simulation

---

## Project Structure

| File              | Description                                          |
| ----------------- | ---------------------------------------------------- |
| `main.c`          | Entry point — initializes blockchain and runs the UI |
| `blockchain.h/c`  | Core blockchain structures and operations            |
| `block.h/c`       | Block-related functions and structures               |
| `transaction.h/c` | Transaction handling and validation                  |
| `merkle.h/c`      | Merkle tree implementation                           |
| `utils.h/c`       | Utility functions (hashing, string operations)       |
| `tests.h/c`       | Test cases for core features                         |
| `ui.h/c`          | Command-line interface logic                         |

---

## How It Works

* The program auto-generates a **Genesis Block** with a system transaction.
* Users add transactions (up to 10 per block).
* When a block is full or manually triggered, a **new block** is created.
* Transactions are hashed into a **Merkle Tree**, with its root hash stored in the block.
* Each block points to the hash of the previous block.
* Blockchain integrity can be verified to detect any tampering.

---

## User Interface

Menu-driven CLI to:

* Add transactions
* Create new blocks
* View blockchain
* Verify integrity
* Simulate attacks
* Run test cases

---

## Blockchain Security

* **Immutability**: Data once written cannot be changed undetected.
* **Integrity Verification**: Uses SHA-256 and Merkle Trees.
* **Tamper Detection**: Simulates basic attacks to show vulnerabilities.
* **Consensus Simulation**: Demonstrates basic agreement among nodes.

---

## Requirements

* GCC / Clang (C compiler)
* OpenSSL library
* POSIX-compliant OS (for threading support)

---

## Compilation

```bash
make
```

## Running

```bash
./blockchain_app
```

---

## Example Usage

1. Start the app
2. Select option 1 to add a transaction (e.g., *Yasmine sends 100 DA to Aicha*)
3. Add more transactions
4. Select option 2 to create a new block
5. Select option 3 to view blockchain
6. Select option 4 to verify chain integrity
7. Exit with option 0

---

## Additional Features

* **Test Suite**: Includes normal and malicious behavior
* **Attack Simulation**: Shows how tampering is detected
* **Consensus Simulation**: Simple peer agreement logic
* **Distributed Replication**: Simulates node-based data redundancy

---

## Reference Assignment

You can refer to the full assignment description for context:
📎 [Download the PDF Assignment](./assignement.pdf) 


