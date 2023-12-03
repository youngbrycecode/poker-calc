#include "Deck.h"
#include "Random.h"

namespace sim
{
    int Deck::OptimalShuffleCount = 45;
    Deck::Deck()
    {
        ReorderAndReset();
    }
    
    Deck::~Deck()
    {
    }
    
    void Deck::ShuffleAndReset(int nTimes, Random& random)
    {
        for (int i = 0; i < nTimes; i++)
        {
            int card1Idx = random.NextInt(NumCardsInDeck);
            int card2Idx = random.NextInt(NumCardsInDeck);

            // Here im not worried if the cards swap with each other. That's just part of the randomness.
            card_t tmpCard1 = mCards[card1Idx];
            mCards[card1Idx] = mCards[card2Idx];
            mCards[card2Idx] = tmpCard1;
        }
        
        Reset();
    }
    
    void Deck::ReorderAndReset() 
    {
        int deckIndex = 0;

        // Create the deck of cards, each card should be in order.
        for (int i = 0; i < (int)Rank::MaxRank; i++)
        {
            for (int j = 0; j < (int)Suit::MaxSuit; j++)
            {
                card_t nextCard = Card::CreateCard((Rank)i, (Suit)j);
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

        card_t nextCard = mCards[mTopOfDeckIndex];
        mTopOfDeckIndex--;
        
        return nextCard;
    }
    
    void Deck::Print(int numCards, std::ostream& outputBuffer)
    {
        outputBuffer << "Printing " << numCards << " from the top of the deck\n";

        int lastCardToPrint = NumCardsInDeck - numCards;
        for (int i = mTopOfDeckIndex; i >= 0 && i >= lastCardToPrint; i--)
        {
            Card::Print(mCards[i], outputBuffer);
        }
    }
}
