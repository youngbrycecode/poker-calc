#pragma once 

#include "Cards.h"
#include "Random.h"

namespace sim 
{
    constexpr int NumCardsInDeck = 52;

    class Deck
    {
        public:
            Deck();
            virtual ~Deck();
            
            /// @brief Shuffles the cards and resets the deck.
            /// @param nTimes 
            /// @param random 
            void ShuffleAndReset(int nTimes, Random& random);
            
            /// @brief Puts the cards back into the starting order.
            void Reset();
            
            /// @brief Draws a single card from the deck if possible.
            /// @return 0 if no cards are left.
            card_t Draw();
            
            /// @brief Prints the top n cards from the deck
            /// @param numCards the number of cards to print
            /// @param outputBuffer 
            void Print(int numCards, std::ostream& outputBuffer);
            
            /// @brief Because random swaps of the cards in the deck
            /// is an irreducable markov chain, it would require infinite
            /// swaps to be sure that any deck permeation N is equally likely
            /// to occur as any other N! permeation.
            /// We'd like to approximate the shuffle such that the odds of reordering any
            /// random card in the deck is 51/52.
            /// (1/2) * log(10; 52) * 52 =/ 45;
            /// So to perform the shuffle, we will swap 45 random cards by default.
            static int OptimalShuffleCount;

        private:
            card_t mCards[NumCardsInDeck];
            int mTopOfDeckIndex;
    };
}