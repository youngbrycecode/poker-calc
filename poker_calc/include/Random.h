#pragma once

#include <stdint.h>

class Random
{
public:
    Random(uint64_t seed = 0)
        : mSeed(seed)
    {
    }

    /// @brief Generates a random number using wyhash. Its a fast algorithm which produces
    /// relatively statistically correct results.
    /// @param max the non-inclusive upper bounds of the random generation.
    /// @return 
    inline int NextInt(int max)
    {
        mSeed = (214013 * mSeed + 2531011);
        return ((mSeed >> 16) & 0x7FFF) % max;
    }

    void SetSeed(uint64_t seed)
    {
        mSeed = seed;
    }

private:
    uint64_t mSeed = 0;
};
