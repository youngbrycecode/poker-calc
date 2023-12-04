#include <iostream>
#include "Poker.h"

using namespace sim;

int main() 
{
    PokerGameState pokerGame(4);
    pokerGame.Deal();
    pokerGame.DealFlop();
    pokerGame.DealTurn();
    pokerGame.DealRiver();
    
    HandClassification classification;
    card_t hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
    card_t hand2 = Card::CreateCard(Rank::Ace, Suit::Diamonds);
    card_t flop1 = Card::CreateCard(Rank::Ace, Suit::Spades);
    card_t flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
    card_t flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
    card_t turn = Card::CreateCard(Rank::Three, Suit::Hearts);
    card_t river = Card::CreateCard(Rank::Ace, Suit::Hearts);

    classification.ClassifyAllCards(hand1, hand2, flop1, flop2, flop3, turn, river);
    classification.PrintAllCardsClassification(std::cout);
    
    return 0;
}