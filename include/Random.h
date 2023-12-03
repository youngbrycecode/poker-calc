#pragma once

#include <stdint.h>

class Random 
{
    public:
        Random(uint64_t seed = 0) 
            : mSeed(seed)
        {
        }

        virtual ~Random()
        {
        }
        
        /// @brief Generates a random number using wyhash. Its a fast algorithm which produces
        /// relatively statistically correct results.
        /// @param max the non-inclusive upper bounds of the random generation.
        /// @return 
        inline int NextInt(int max)
        {
            mSeed += 0x60bee2bee120fc15;
            __uint128_t tmp = (__uint128_t)mSeed * 0xa3b195354a39b70d;

            uint64_t m1 = (tmp >> 64) ^ tmp;
            tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
            uint64_t m2 = (tmp >> 64) ^ tmp;

            // Crude way to convert to the expected range, but it
            // should be okay for random shuffles (because I said so).
            return m2 % max;
        }
        
        void SetSeed(uint64_t seed) 
        {
            mSeed = seed;
        }
        
    private:
        uint64_t mSeed = 0;
};
