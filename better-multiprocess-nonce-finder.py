import multiprocessing
import hashlib
import numpy as np
import string
import time
import signal
import sys

rng = np.random.default_rng()  # Faster random number generator

def generate_nonce(length=8):
    return ''.join(rng.choice(list(string.ascii_letters + string.digits), length))

def mine(best_leading_zeros, best_nonce, best_hash, lock, process_id):
    local_best_nonce = ""
    local_best_hash = ""
    local_best_leading_zeros = 0

    last_update_time = time.time()
    update_interval = np.random.uniform(50, 70)  # Staggered update times

    def handle_exit(signum, frame):
        """Handles Ctrl+C (SIGINT) and prints the local best result before exiting."""
        print(f"[Process {process_id}] Exiting... Best found: {local_best_nonce} -> {local_best_hash} ({local_best_leading_zeros} leading zeros)")
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_exit)  # Register Ctrl+C handler

    while True:
        nonce = generate_nonce()
        input_str = f"Ethan Anabela {nonce}"
        hash_result = hashlib.sha256(input_str.encode()).hexdigest()
        leading_zeros = len(hash_result) - len(hash_result.lstrip('0'))

        if leading_zeros > local_best_leading_zeros:
            local_best_nonce = nonce
            local_best_hash = hash_result
            local_best_leading_zeros = leading_zeros

        if time.time() - last_update_time >= update_interval:
            last_update_time = time.time()
            update_interval = np.random.uniform(50, 70)

            with lock:
                if local_best_leading_zeros > best_leading_zeros.value:
                    best_leading_zeros.value = local_best_leading_zeros
                    best_nonce.value = local_best_nonce.encode('utf-8')
                    best_hash.value = local_best_hash.encode('utf-8')
                    print(f"[Process {process_id}] New best: {local_best_nonce} -> {local_best_hash} ({local_best_leading_zeros} leading zeros)")

        if best_leading_zeros.value >= 10:
            break

if __name__ == "__main__":
    best_leading_zeros = multiprocessing.Value("i", 0)  # No Manager overhead
    best_nonce = multiprocessing.Array("c", 64)  # Shared memory for nonce
    best_hash = multiprocessing.Array("c", 64)  # Shared memory for hash
    lock = multiprocessing.Lock()

    num_physical_cores = multiprocessing.cpu_count() // 2  # Use physical cores
    num_processes = min(num_physical_cores, 6)  # Max 6 processes

    processes = []
    for i in range(num_processes):
        p = multiprocessing.Process(target=mine, args=(best_leading_zeros, best_nonce, best_hash, lock, i))
        processes.append(p)
        p.start()

    try:
        for p in processes:
            p.join()
    except KeyboardInterrupt:
        print("\n[Main] Ctrl+C detected. Stopping all processes...")
        for p in processes:
            p.terminate()  # Ensure all processes stop
        print(f"\n[Main] Final best result: Nonce: {best_nonce.value.decode('utf-8')}, Hash: {best_hash.value.decode('utf-8')}, Leading Zeros: {best_leading_zeros.value}")

