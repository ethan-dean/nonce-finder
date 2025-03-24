#include "miner.h"
#include "sha256.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>

// Global shared state.
std::mutex global_mutex;
int global_best_leading_zeros = 0;
std::string global_best_nonce;
std::string global_best_hash;
std::atomic<bool> stopFlag(false);

// Signal handler for SIGINT.
void signal_handler(int /*signum*/) {
    stopFlag.store(true);
    std::cout << "\n[Main] SIGINT received. Stopping all threads..." << std::endl;
}

// Returns a random alphanumeric nonce of given length.
std::string generate_nonce(size_t length = 8) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<> dist(0, static_cast<int>(chars.size() - 1));
    std::string nonce;
    nonce.reserve(length);
    for (size_t i = 0; i < length; ++i)
        nonce.push_back(chars[dist(rng)]);
    return nonce;
}

// Counts the number of leading '0' characters in a hex string.
inline int count_leading_zeros(const std::string &hex) {
    int count = 0;
    for (char c : hex) {
        if (c == '0') count++;
        else break;
    }
    return count;
}

// The mining loop run by each thread.
void mine(int process_id) {
    std::string local_best_nonce;
    std::string local_best_hash;
    int local_best_leading_zeros = 0;

    auto last_update_time = std::chrono::steady_clock::now();
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<> dist_interval(50.0, 70.0);
    double update_interval = dist_interval(rng);

    while (!stopFlag.load()) {
        std::string nonce = generate_nonce();
        std::string input_str = "Ethan Anabela " + nonce;
        std::string hash_result = sha256::hash(input_str);
        int leading_zeros = count_leading_zeros(hash_result);

        if (leading_zeros > local_best_leading_zeros) {
            local_best_nonce = nonce;
            local_best_hash = hash_result;
            local_best_leading_zeros = leading_zeros;
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last_update_time;
        if (elapsed.count() >= update_interval) {
            last_update_time = now;
            update_interval = dist_interval(rng);
            {
                std::lock_guard<std::mutex> lock(global_mutex);
                if (local_best_leading_zeros > global_best_leading_zeros) {
                    global_best_leading_zeros = local_best_leading_zeros;
                    global_best_nonce = local_best_nonce;
                    global_best_hash = local_best_hash;
                    std::cout << "[Thread " << process_id << "] New best: "
                              << local_best_nonce << " -> " << local_best_hash
                              << " (" << local_best_leading_zeros << " leading zeros)" << std::endl;
                }
            }
        }
        {
            std::lock_guard<std::mutex> lock(global_mutex);
            if (global_best_leading_zeros >= 10)
                break;
        }
    }

    std::cout << "[Thread " << process_id << "] Exiting... Best found: "
              << local_best_nonce << " -> " << local_best_hash
              << " (" << local_best_leading_zeros << " leading zeros)" << std::endl;
}
