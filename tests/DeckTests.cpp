#include <gtest/gtest.h>
#include "Deck.h"

using namespace sim;

TEST(DeckTests, TestCreateCards)
{
    // Test a variety of cards and make sure we get the right values for the card.
    card_t twoClubs = Card::CreateCard(Rank::Two, Suit::Clubs);
    ASSERT_EQ(Card::GetRank(twoClubs), Rank::Two);
    ASSERT_EQ(Card::GetSuit(twoClubs), Suit::Clubs);

    card_t aceSpades = Card::CreateCard(Rank::Ace, Suit::Spades);
    ASSERT_EQ(Card::GetRank(aceSpades), Rank::Ace);
    ASSERT_EQ(Card::GetSuit(aceSpades), Suit::Spades);

    card_t eightHearts = Card::CreateCard(Rank::Eight, Suit::Hearts);
    ASSERT_EQ(Card::GetRank(eightHearts), Rank::Eight);
    ASSERT_EQ(Card::GetSuit(eightHearts), Suit::Hearts);

    card_t queenDiamonds = Card::CreateCard(Rank::Queen, Suit::Diamonds);
    ASSERT_EQ(Card::GetRank(queenDiamonds), Rank::Queen);
    ASSERT_EQ(Card::GetSuit(queenDiamonds), Suit::Diamonds);
}

TEST(DeckTests, TestDeckResetOrder)
{
    Deck deck;
    deck.Reset();

    for (int i = (int)Rank::MaxRank - 1; i >= 0; i--)
    {
        for (int j = (int)Suit::MaxSuit - 1; j >= 0; j--)
        {
            card_t card = Card::CreateCard((Rank)i, (Suit)j);
            ASSERT_EQ(card, deck.Draw());
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}