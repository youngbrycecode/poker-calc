#pragma once 

#include "Cards.h"
#include "Random.h"
#include <limits>

namespace sim
{
    constexpr int NumCardsInDeck = 52;
    constexpr int NotACard = std::numeric_limits<card_t>::max();

    class Deck
    {
    public:
        Deck()
        {
            mCards = (card_t*)malloc(sizeof(card_t) * 128);
            ReorderAndReset();
        }

        virtual ~Deck()
        {
            free(mCards);
        }

        /// @brief Shuffles the cards and resets the deck.
        /// @param nTimes 
        /// @param random 
        inline void ShuffleAndReset(int nTimes, Random& random)
        {
            for (int i = 0; i < nTimes; i++)
            {
                const int card1Idx = random.NextInt(NumCardsInDeck);
                const int card2Idx = random.NextInt(NumCardsInDeck);

                // Here im not worried if the cards swap with each other. That's just part of the randomness.
                const card_t tmpCard1 = mCards[card1Idx];
                mCards[card1Idx] = mCards[card2Idx];
                mCards[card2Idx] = tmpCard1;
            }

            Reset();
        }

        /// @brief Puts the cards back into the starting order.
        inline void ReorderAndReset()
        {
            int deckIndex = 0;

            // Create the deck of cards, each card should be in order.
            for (int i = 0; i < static_cast<int>(Rank::MaxRank); i++)
            {
                for (int j = 0; j < static_cast<int>(Suit::MaxSuit); j++)
                {
                    const card_t nextCard = Card::CreateCard(static_cast<Rank>(i), static_cast<Suit>(j));
                    mCards[deckIndex] = nextCard;
                    deckIndex++;
                }
            }

            Reset();
        }

        /// @brief Returns all the cards to the deck in the last order they were seen in.
        inline void Reset()
        {
            mTopOfDeckIndex = NumCardsInDeck - 1;
        }

        /// @brief Draws a single card from the deck if possible.
        /// @return 0 if no cards are left.
        inline card_t Draw()
        {
            if (mTopOfDeckIndex < 0)
            {
                return NotACard;
            }

            const card_t nextCard = mCards[mTopOfDeckIndex];
            mTopOfDeckIndex--;

            return nextCard;
        }

        /// @brief Prints the top n cards from the deck
        /// @param numCards the number of cards to print
        /// @param outputBuffer 
        void Print(int numCards, std::ostream& outputBuffer) const;

        /// @brief Because random swaps of the cards in the deck
        /// is a non-reducible markov chain, it would require infinite
        /// swaps to be sure that any deck permeation N is equally likely
        /// to occur as any other N! permeation.
        /// We'd like to approximate the shuffle such that the odds of reordering any
        /// random card in the deck is 51/52.
        /// (1/2) * log(10; 52) * 52 =/ 45;
        /// So to perform the shuffle, we will swap 45 random cards by default.
        static constexpr int OptimalShuffleCount = 45;

    private:
        card_t* mCards;
        int mTopOfDeckIndex;
    };
}