#include <iostream>
#include <fstream>

#include "LookupTables.h"
#include "Poker.h"

using namespace sim;

int main() 
{
    InitLookupTables();

    const card_t h1 = Card::CreateCard(Rank::Two, Suit::Spades);
    const card_t h2 = Card::CreateCard(Rank::Three, Suit::Hearts);
    const card_t f1 = Card::CreateCard(Rank::Four, Suit::Clubs);
    const card_t f2 = Card::CreateCard(Rank::Five, Suit::Diamonds);
    const card_t f3 = Card::CreateCard(Rank::Six, Suit::Spades);

    HandClassification classification;
    classification.ClassifyAllCardsFast(h1, h2, f1, f2, f3, NotACard, NotACard);

    return 0;
}