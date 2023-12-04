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
    
    return 0;
}