# Nonce Finder

This repository contains two implementations of a nonce-finding algorithm that searches for a SHA-256 hash with a specified number of leading zeros. The project is for FIN4450 (Spring 2025) at the University of Central Florida.

## Overview

The script continuously hashes the string `"Ethan Anabela [ANY_NONCE_STRING]"` using SHA-256 and tracks the nonce that results in the most leading zeros. The search stops once a hash with at least 10 leading zeros is found.

## Files

- **`multiprocess-nonce-finder.py`**: Uses the `multiprocessing` module to run the nonce-finding algorithm across multiple CPU cores for maximum performance.
- **`multithread-nonce-finder.py`**: Uses the `threading` module to run the algorithm with multiple threads, but due to Python’s Global Interpreter Lock (GIL), it runs on a single core.

## Requirements

This project requires Python 3.x. No additional dependencies are needed.

## Usage

Run either script in a terminal:

```sh
python multiprocess-nonce-finder.py
```

or

```sh
python multithread-nonce-finder.py
```

The script will output updates whenever a new best nonce is found and stop when a hash with 10 leading zeros is discovered.
