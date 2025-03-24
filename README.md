# Nonce Finder

This repository contains code to continuously hash the string `"Ethan Anabela [ANY_NONCE_STRING]"` using SHA-256 and tracks the nonce that results in the most leading zeros. The search stops once a hash with at least 10 leading zeros is found.

## Python Implementation

This repository contains three implementations of a nonce-finding algorithm that searches for a SHA-256 hash with a specified number of leading zeros. The project is for FIN4450 (Spring 2025) at the University of Central Florida.

### Files

- **`multithread-nonce-finder.py`**: Uses the `threading` module to run the algorithm with multiple threads, but due to Python’s Global Interpreter Lock (GIL), it runs on a single core.
- **`multiprocess-nonce-finder.py`**: Uses the `multiprocessing` module to run the nonce-finding algorithm across multiple CPU cores for maximum performance.
- **`better-multiprocess-nonce-finder.py`**: Uses the `multiprocessing` module to run the optimized version of the nonce-finding algorithm across multiple CPU cores for maximum performance.

### Requirements

This project requires Python 3.x.

#### Unoptimized (multithread/multiprocess)

No additional dependencies are needed.

#### Optimized (better-multiprocess)

- NumPy

### Usage

Run any of the Python scripts in a terminal:

```sh
python [FILENAME].py
```

The script will output updates whenever a new best nonce is found and stop when a hash with 10 leading zeros is discovered.

---

## C++ Implementation

A C++ version of the nonce-finding algorithm is also provided. This implementation is designed for maximum performance, using only the C++ standard library along with a self-contained SHA-256 implementation (adapted from picoSHA2). Since SHA-256 is standardized, this implementation produces identical hashes to Python’s `hashlib.sha256`.

### Files

- **`sha256.h`** and **`sha256.cpp`**:  
  Contain the SHA-256 implementation.

- **`miner.h`** and **`miner.cpp`**:  
  Contain the mining algorithm, global shared state, and threading logic.

- **`main.cpp`**:  
  Contains the main entry point that launches mining threads and prints the results.

### Compilation

To compile the C++ implementation with maximum optimization using a modern C++ compiler (e.g., g++), navigate to the project directory and run:

```sh
g++ -std=c++17 -O3 -march=native main.cpp miner.cpp sha256.cpp -o nonce_finder
```

This command instructs the compiler to:
- Use the C++17 standard.
- Optimize the code aggressively (`-O3`).
- Generate code tuned for your host machine (`-march=native`).

### Usage

After compiling, run the executable from your terminal:

```sh
./nonce_finder
```

The program will output updates when a new best nonce is found and will terminate once a hash with at least 10 leading zeros is achieved.

### Performance

The C++ version benefits from being compiled to native code, avoiding Python interpreter overhead, and is generally 5–10 times faster than the Python implementations for this CPU-bound task.
