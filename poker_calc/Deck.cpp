#include "Deck.h"
#include "Random.h"

namespace sim
{
    Deck::Deck()
    {
        ReorderAndReset();
    }
    
    void Deck::ShuffleAndReset(int nTimes, Random& random)
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
    
    void Deck::ReorderAndReset() 
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

    void Deck::Reset()
    {
        mTopOfDeckIndex = NumCardsInDeck - 1;
    }
    
    card_t Deck::Draw()
    {
        if (mTopOfDeckIndex < 0) 
        {
            return NotACard;
        }

        const card_t nextCard = mCards[mTopOfDeckIndex];
        mTopOfDeckIndex--;
        
        return nextCard;
    }
    
    void Deck::Print(int numCards, std::ostream& outputBuffer) const
    {
        outputBuffer << "Printing " << numCards << " from the top of the deck\n";

        const int lastCardToPrint = NumCardsInDeck - numCards;
        for (int i = mTopOfDeckIndex; i >= 0 && i >= lastCardToPrint; i--)
        {
            Card::Print(mCards[i], outputBuffer);
        }
    }
}
