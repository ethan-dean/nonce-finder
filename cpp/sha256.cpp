#include "sha256.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>

// This is an adapted version of a public domain picoSHA2 implementation.
namespace sha256 {

    using uint32 = unsigned int;
    using uint8 = unsigned char;

    inline uint32 ch(uint32 x, uint32 y, uint32 z) {
        return (x & y) ^ ((~x) & z);
    }
    inline uint32 maj(uint32 x, uint32 y, uint32 z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }
    inline uint32 rotr(uint32 x, unsigned n) {
        return (x >> n) | (x << (32 - n));
    }
    inline uint32 bsig0(uint32 x) {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }
    inline uint32 bsig1(uint32 x) {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }
    inline uint32 ssig0(uint32 x) {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }
    inline uint32 ssig1(uint32 x) {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    const uint32 K[64] = {
        0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,
        0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
        0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
        0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
        0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,
        0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
        0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,
        0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
        0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
        0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
        0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,
        0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
        0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,
        0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
        0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
        0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
    };

    std::string hash(const std::string& src) {
        std::vector<uint8> data(src.begin(), src.end());
        uint64_t bit_len = data.size() * 8;
        // Append the '1' bit (0x80)
        data.push_back(0x80);
        // Append zeros until message length in bits ≡ 448 mod 512.
        while ((data.size() * 8) % 512 != 448)
            data.push_back(0x00);
        // Append the original message length as a 64-bit big-endian integer.
        for (int i = 7; i >= 0; --i)
            data.push_back((bit_len >> (i * 8)) & 0xff);

        uint32 h[8] = {
            0x6a09e667u,
            0xbb67ae85u,
            0x3c6ef372u,
            0xa54ff53au,
            0x510e527fu,
            0x9b05688cu,
            0x1f83d9abu,
            0x5be0cd19u
        };

        const size_t chunk_size = 64;
        for (size_t chunk_start = 0; chunk_start < data.size(); chunk_start += chunk_size) {
            uint32 w[64] = {0};
            // Convert chunk to 16 big-endian 32-bit words.
            for (size_t i = 0; i < 16; ++i) {
                size_t idx = chunk_start + i * 4;
                w[i] = (data[idx] << 24) | (data[idx+1] << 16) | (data[idx+2] << 8) | (data[idx+3]);
            }
            // Extend to 64 words.
            for (size_t i = 16; i < 64; ++i)
                w[i] = ssig1(w[i-2]) + w[i-7] + ssig0(w[i-15]) + w[i-16];

            uint32 a = h[0], b = h[1], c = h[2], d = h[3];
            uint32 e = h[4], f = h[5], g = h[6], h_val = h[7];

            for (size_t i = 0; i < 64; ++i) {
                uint32 temp1 = h_val + bsig1(e) + ch(e, f, g) + K[i] + w[i];
                uint32 temp2 = bsig0(a) + maj(a, b, c);
                h_val = g; g = f; f = e;
                e = d + temp1; d = c; c = b; b = a;
                a = temp1 + temp2;
            }
            h[0] += a; h[1] += b; h[2] += c; h[3] += d;
            h[4] += e; h[5] += f; h[6] += g; h[7] += h_val;
        }

        std::ostringstream oss;
        for (int i = 0; i < 8; i++)
            oss << std::hex << std::setw(8) << std::setfill('0') << h[i];
        return oss.str();
    }
}
