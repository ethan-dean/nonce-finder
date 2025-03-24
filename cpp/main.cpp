#include "miner.h"
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>

// Forward declaration of the signal handler from miner.cpp.
extern void signal_handler(int);

// Declare extern globals defined in miner.cpp
extern std::mutex global_mutex;
extern int global_best_leading_zeros;
extern std::string global_best_nonce;
extern std::string global_best_hash;

int main() {
    std::signal(SIGINT, signal_handler);

    unsigned int hardware_threads = std::thread::hardware_concurrency();
    unsigned int physical_cores = hardware_threads / 2;
    unsigned int num_threads = std::min(physical_cores, 6u);
    if (num_threads == 0) num_threads = 1;

    std::cout << "[Main] Starting " << num_threads << " mining thread(s)..." << std::endl;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i)
        threads.emplace_back(mine, i);

    for (auto &t : threads)
        if (t.joinable())
            t.join();

    {
        std::lock_guard<std::mutex> lock(global_mutex);
        std::cout << "\n[Main] Final best result: Nonce: " << global_best_nonce
                  << ", Hash: " << global_best_hash
                  << ", Leading Zeros: " << global_best_leading_zeros << std::endl;
    }
    return 0;
}
