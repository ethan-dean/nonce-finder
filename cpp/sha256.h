#ifndef SHA256_H
#define SHA256_H

#include <string>

namespace sha256 {
    // Returns the SHA-256 hash (as a hex string) for the given input.
    std::string hash(const std::string &src);
}

#endif // SHA256_H
