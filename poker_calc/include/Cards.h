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

            static Rank GetRank(card_t card);
            static Suit GetSuit(card_t card);
    };
}