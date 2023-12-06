#include "Deck.h"
#include "Random.h"

namespace sim
{
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
