#pragma once

#include <ostream>
#include <stdint.h>

namespace sim
{
    typedef uint8_t card_t;

    enum class Rank
    {
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Jack,
        Queen,
        King,
        Ace,
        MaxRank
    };

    enum class Suit
    {
        Spades,
        Hearts,
        Clubs,
        Diamonds,
        MaxSuit
    };

    /// @brief Utilities dealing with cards.
    class Card
    {
    public:
        /// @brief Cards are encoded into a single integer for compression 
        /// @param rank 
        /// @param suit 
        /// @return 
        static card_t CreateCard(Rank rank, Suit suit);

        static void Print(card_t card, std::ostream& outBuffer);
        static void PrintRank(card_t card, std::ostream& outBuffer);
        static void PrintSuit(card_t card, std::ostream& outBuffer);

        /// @brief 1111 xxxx
        static constexpr card_t RankMask = 0x3C;

        /// @brief xxxx 11xx
        static constexpr card_t SuitMask = 3;

        static constexpr int RankBits = 4;
        static constexpr int SuitBits = 2;

        inline static Rank GetRank(card_t card)
        {
            return static_cast<Rank>((card & RankMask) >> SuitBits);
        }

        inline static Suit GetSuit(card_t card)
        {
            return static_cast<Suit>(card & SuitMask);
        }
    };
}