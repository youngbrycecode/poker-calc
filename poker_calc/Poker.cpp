#include "Poker.h"

namespace sim 
{
    PokerGameState::PokerGameState(int numPlayers, int seed)
        : mHands(numPlayers),
          mRandom(seed)
    {
    }
    
    PokerGameState::~PokerGameState()
    {
    }
    
    void PokerGameState::Deal()
    {
        for (int i = 0; i < Hand::HandSize; i++)
        {
            for (auto& hand : mHands)
            {
                card_t nextCard = mDeck.Draw();
                hand.DealCard(nextCard);
            }
        }
    }

    void PokerGameState::DealFlop()
    {
        // Discard the top card (that's how the game is played)
        mDeck.Draw();
        mFlop[0] = mDeck.Draw();
        mFlop[1] = mDeck.Draw();
        mFlop[2] = mDeck.Draw();
    }
    
    void PokerGameState::DealTurn()
    {
        mDeck.Draw();
        mTurn = mDeck.Draw();
    }
    
    void PokerGameState::DealRiver()
    {
        mDeck.Draw();
        mRiver = mDeck.Draw();
    }
    
    void PokerGameState::Reset()
    {
        mDeck.ShuffleAndReset(Deck::OptimalShuffleCount, mRandom);
    }
    
    PokerGameState::Hand::Hand()
    {
    }
    
    PokerGameState::Hand::~Hand()
    {
    }
    
    void PokerGameState::Hand::DealCard(card_t card)
    {
        if (mHandIndex >= HandSize)
        {
            throw std::out_of_range("Players can only be dealt " 
                                    + std::to_string(HandSize) 
                                    + " cards per round");
        }
        
        mHand[mHandIndex] = card;
        mHandIndex++;
    }
    
    void PokerGameState::Hand::ClearHand()
    {
        mHandIndex = 0;
    }
}