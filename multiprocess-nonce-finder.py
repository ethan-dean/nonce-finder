import multiprocessing
import hashlib
import random
import string

def generate_nonce(length=8):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def mine(best_nonce, best_hash, best_leading_zeros, lock):
    while True:
        nonce = generate_nonce()
        input_str = f"Ethan Anabela {nonce}"
        hash_result = hashlib.sha256(input_str.encode()).hexdigest()
        leading_zeros = len(hash_result) - len(hash_result.lstrip('0'))

        with lock:
            if leading_zeros > best_leading_zeros.value:
                best_leading_zeros.value = leading_zeros
                best_nonce.value = nonce
                best_hash.value = hash_result
                print(f"New best: {best_nonce.value} -> {best_hash.value} ({best_leading_zeros.value} leading zeros)")

            if best_leading_zeros.value >= 10:
                break

if __name__ == "__main__":
    manager = multiprocessing.Manager()
    best_nonce = manager.Value("s", "")
    best_hash = manager.Value("s", "")
    best_leading_zeros = manager.Value("i", 0)
    lock = manager.Lock()

    num_processes = multiprocessing.cpu_count()  # Use all available cores
    processes = []

    for _ in range(min(6, num_processes)):  # Run up to 6 processes or max cores
        p = multiprocessing.Process(target=mine, args=(best_nonce, best_hash, best_leading_zeros, lock))
        processes.append(p)
        p.start()

    for p in processes:
        p.join()

    print(f"Final result: Nonce: {best_nonce.value}, Hash: {best_hash.value}, Leading Zeros: {best_leading_zeros.value}")
