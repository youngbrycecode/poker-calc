#include "Cards.h"
#include <vector>
#include <string>

namespace sim
{
    /// @brief 1111 xxxx
    constexpr card_t RankMask = 0x3C;
    
    /// @brief xxxx 11xx
    constexpr card_t SuitMask = 3;
    
    constexpr int RankBits = 4;
    constexpr int SuitBits = 2;

    static std::vector<std::string> RankStrings 
    {
        "Two",
        "Three",
        "Four",
        "Five",
        "Six",
        "Seven",
        "Eight",
        "Nine",
        "Ten",
        "Jack",
        "Queen",
        "King",
        "Ace"
    };

    static std::vector<std::string> SuitStrings 
    {
        "Spades",
        "Hearts",
        "Clubs",
        "Diamonds",
    };
    
    card_t Card::CreateCard(Rank rank, Suit suit) 
    {
        // Use the first couple bits for the rank, and use the 5th and 6th bits for the suit.
        card_t resultCard = (card_t)suit;
        resultCard |= ((card_t)rank) << SuitBits;

        return resultCard;
    }
    
    void Card::Print(card_t card, std::ostream& outBuffer)
    {
        Rank rank = GetRank(card);
        Suit suit = GetSuit(card);

        outBuffer << RankStrings[(int)rank] << 
                        " Of " << 
                        SuitStrings[(int)suit] 
                        << "\n";
    }
    
    void Card::PrintRank(card_t card, std::ostream& outBuffer)
    {
        outBuffer << RankStrings[(int)GetRank(card)] << "\n";
    }
    
    void Card::PrintSuit(card_t card, std::ostream& outBuffer)
    {
        outBuffer << SuitStrings[(int)GetSuit(card)] << "\n";
    }
    
    Rank Card::GetRank(card_t card)
    {
        return (Rank)((card & RankMask) >> SuitBits);
    }
    
    Suit Card::GetSuit(card_t card)
    {
        return (Suit)(card & SuitMask);
    }
}