#include <iostream>
#include <fstream>

#include "LookupTables.h"
#include "Poker.h"

using namespace sim;

int main() 
{
    InitLookupTables();

    const card_t h1 = Card::CreateCard(Rank::Four, Suit::Spades);
    const card_t h2 = Card::CreateCard(Rank::Six, Suit::Spades);
    const card_t f1 = Card::CreateCard(Rank::Ace, Suit::Spades);
    const card_t f2 = Card::CreateCard(Rank::Queen, Suit::Spades);
    const card_t f3 = Card::CreateCard(Rank::Five, Suit::Spades);
    //const card_t t = Card::CreateCard(Rank::King, Suit::Clubs);
    //const card_t r = Card::CreateCard(Rank::Ten, Suit::Hearts);

    HandClassification classification;
    classification.ClassifyAllCardsFast(h1, h2, f1, f2, f3, NotACard, NotACard);

    classification.PrintAllCardsClassification(std::cout);

    CleanupLookupTables();
    return 0;
}