import threading
import hashlib
import random
import string

# Shared variables
best_nonce = None
best_hash = None
best_leading_zeros = 0
lock = threading.Lock()

# Function to generate a random nonce
def generate_nonce(length=8):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

# Function to compute SHA-256 hash and track the best nonce
def mine():
    global best_nonce, best_hash, best_leading_zeros
    
    while best_leading_zeros < 10:
        nonce = generate_nonce()
        input_str = f"Ethan Anabela {nonce}"
        hash_result = hashlib.sha256(input_str.encode()).hexdigest()
        leading_zeros = len(hash_result) - len(hash_result.lstrip('0'))
        
        with lock:
            if leading_zeros > best_leading_zeros:
                best_leading_zeros = leading_zeros
                best_nonce = nonce
                best_hash = hash_result
                print(f"New best: {best_nonce} -> {best_hash} ({best_leading_zeros} leading zeros)")
        
        if best_leading_zeros >= 10:
            break

# Create and start 6 threads
threads = []
for _ in range(6):
    thread = threading.Thread(target=mine)
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

print(f"Final result: Nonce: {best_nonce}, Hash: {best_hash}, Leading Zeros: {best_leading_zeros}")

