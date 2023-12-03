#include <iostream>
#include "Cards.h"
#include "Deck.h"

using namespace sim;

int main() 
{
    Random random(1);

    Deck deck;
    deck.ShuffleAndReset(Deck::OptimalShuffleCount, random);
    
    deck.Print(10, std::cout);
    return 0;
}