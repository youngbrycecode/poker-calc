#pragma once

#include <vector>
#include <memory>

#include "Random.h"
#include "Deck.h"

namespace sim 
{
    /// @brief The clasifications (and rankings of poker hands)
    enum class HandClass 
    {
        // Highest card in your hand.
        HighCard,
        // Two matching cards.
        OnePair,
        // Two matching cards of separate ranks.
        TwoPair,
        // Three cards of the same rank.
        ThreeOfAKind,
        // Five cards in a row with different suits.
        // Aces can be either used as the high or low card. (1/13)
        Straight,
        // Five cards of the same suit.
        Flush,
        // Three of a kind plus a pair.
        FullHouse,
        // Four cards of the same rank.
        FourOfAKind,
        // Five cards in a row with the same suit.
        StraightFlush,
        // Straight flush with 10-Ace
        RoyalFlush,
        MaxClassification
    };
    
    #pragma region HAND_TYPE_STRUCTS
    
    struct tHighCardData 
    {
        Rank HighRank;
    };
    
    struct tOnePairData
    {
        Rank HighRank;
    };
    
    struct tTwoPairData 
    {
        Rank SecondPairRank;
    };
    
    struct tThreeOfAKindData
    {
        Rank HighRank;
    };
    
    struct tStraightData
    {
        Rank HighRank;
    };

    struct tFlushData
    {
        Suit FlushSuit;
    };
    
    struct tFourOfAKindData
    {
        Rank HighRank;
    };
    
    struct tStraightFlushData
    {
        Suit FlushSuit;
        Rank HighRank;
    };

    struct tClassificationData
    {
        HandClass HandClassification;

        /// @brief 2^6 is 64, so we store an array of 64 cards.
        /// When starting the classification, instead of sorting the cards,
        /// its faster to iterate through a small list.
        /// The indices of this array are set at the start of the function call, then
        /// they must be cleared at the end.
        /// NOTE: this array should not be written to without restoring it back to an all zero state.
        uint8_t AllCards[64];

        // Classification data.
        tHighCardData HighCard;
        tOnePairData OnePair;
        tTwoPairData TwoPair;
        tThreeOfAKindData ThreeOfAKind;
        tStraightData Straight;
        tFlushData Flush;
        tFourOfAKindData FourOfAKind;
        tStraightFlushData StraightFlush;

        card_t HighestCard;
        card_t LowestCard;

        /// @brief Bit fields computed during the preprocessing phase
        /// These store for each rank 4 bits for which suits exist
        /// And for each suit, 13 bits for each rank card that exists having that suit.
        uint32_t RankBitFields[static_cast<int>(Rank::MaxRank)];
        uint32_t SuitBitFields[static_cast<int>(Suit::MaxSuit)];
        
        /// @brief This rank bit field is independent of suit.
        /// A value is always set if the rank appears.
        uint32_t RankBitField;
        
        // The number of cards at the current rank.
        uint32_t RankCardCount[static_cast<int>(Rank::MaxRank)];
        uint32_t SuitCardCount[static_cast<int>(Suit::MaxSuit)];
    };
    
    #pragma endregion HAND_TYPE_STRUCTS
    
    class HandClassification 
    {
        public:
            HandClassification() = default;

            /// @brief Classify the cards. Creates classifications for 
            /// preflop, postflop, with turn, and with river.
            /// @param hand1 
            /// @param hand2 
            /// @param flop1 
            /// @param flop2 
            /// @param flop3 
            /// @param turn 
            /// @param river 
            void ClassifyAllCards(card_t hand1, card_t hand2, 
                card_t flop1, card_t flop2, card_t flop3,
                card_t turn, card_t river);
            
            void PrintAllCardsClassification(std::ostream& outputStream);

        private:
            tClassificationData mAllCardsClassData;
    };

    class PokerGameState
    {
        public:
            PokerGameState(int numPlayers, int seed = 100);
            virtual ~PokerGameState();
            
            /// @brief Deal cards to every player.
            void Deal();
            
            /// @brief Discard top card and reveal three cards (flop)
            void DealFlop();
            
            /// @brief Burn a card, then reveal the next besides the flop.
            void DealTurn();
            
            /// @brief Deal the last card on the table.
            void DealRiver();
            
            /// @brief Shuffles the cards, and resets the game to the starting state.
            void Reset();

        private:
            /// @brief The three cards played after deal.
            card_t mFlop[3];
            card_t mTurn;
            card_t mRiver;

            Deck mDeck;
            Random mRandom;

            /// @brief Class to store information about a hand in poker.
            class Hand 
            {
                public:
                    Hand() = default;
                    
                    /// @brief Puts a card in the hand.
                    /// @param card 
                    void DealCard(card_t card);
                    
                    /// @brief Resets the hand for the next deal cycle.
                    void ClearHand();
                    
                    const HandClassification& GetAllCardsClassification(card_t flop1, card_t flop2, card_t flop3,
                        card_t turn, card_t river)
                    {
                        mHandClassification.ClassifyAllCards(mHand[0], mHand[1],
                            flop1, flop2, flop3, turn, river);
                        
                        return mHandClassification;
                    }

                    static constexpr int HandSize = 2;

                private:
                    card_t mHand[HandSize];
                    int mHandIndex = 0;
                    HandClassification mHandClassification;
            };

            std::vector<Hand> mHands;
    };
}