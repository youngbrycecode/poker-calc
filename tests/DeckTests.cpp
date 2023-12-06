#include <gtest/gtest.h>
#include <array>
#include "Deck.h"

using namespace sim;

namespace
{
    void ValidateDeck(Deck& deck);
    void DoubleCompareEq(double d1, double d2, double thresh);
}

TEST(DeckTests, TestCreateCards)
{
    // Test a variety of cards and make sure we get the right values for the card.
    const card_t twoClubs = Card::CreateCard(Rank::Two, Suit::Clubs);
    ASSERT_EQ(Card::GetRank(twoClubs), Rank::Two);
    ASSERT_EQ(Card::GetSuit(twoClubs), Suit::Clubs);

    const card_t aceSpades = Card::CreateCard(Rank::Ace, Suit::Spades);
    ASSERT_EQ(Card::GetRank(aceSpades), Rank::Ace);
    ASSERT_EQ(Card::GetSuit(aceSpades), Suit::Spades);

    const card_t eightHearts = Card::CreateCard(Rank::Eight, Suit::Hearts);
    ASSERT_EQ(Card::GetRank(eightHearts), Rank::Eight);
    ASSERT_EQ(Card::GetSuit(eightHearts), Suit::Hearts);

    const card_t queenDiamonds = Card::CreateCard(Rank::Queen, Suit::Diamonds);
    ASSERT_EQ(Card::GetRank(queenDiamonds), Rank::Queen);
    ASSERT_EQ(Card::GetSuit(queenDiamonds), Suit::Diamonds);
}

/// @brief Test the intentional property that cards of greater rank will be higher
/// values than cards of any other rank.
TEST(DeckTests, CardOrderTests)
{
    // Iterate through every rank, and make sure that every card in this rank is 
    card_t previousMaxCard = 0;

    for (int i = 0; i < static_cast<int>(Rank::MaxRank); i++)
    {
        card_t maxCardAtRank = 0;
        for (int j = 0; j < static_cast<int>(Suit::MaxSuit); j++)
        {
            const card_t currentCard = Card::CreateCard(static_cast<Rank>(i), static_cast<Suit>(j));
            if (currentCard > maxCardAtRank)
            {
                maxCardAtRank = currentCard;
            }
        }
        
        ASSERT_GT(maxCardAtRank, previousMaxCard);
        previousMaxCard = maxCardAtRank;
    }
}

TEST(DeckTests, TestDeckResetOrder)
{
    Deck deck;
    deck.ReorderAndReset();

    for (int i = static_cast<int>(Rank::MaxRank) - 1; i >= 0; i--)
    {
        for (int j = static_cast<int>(Suit::MaxSuit) - 1; j >= 0; j--)
        {
            card_t card = Card::CreateCard(static_cast<Rank>(i), static_cast<Suit>(j));
            ASSERT_EQ(card, deck.Draw());
        }
    }
}

TEST(DeckTests, SufficientShuffleRandomness)
{
    // Shuffle the deck thousands of times counting the number of times each card appeared at each position.
    // Ensure that (within a threshold, we have an equal probability of getting any card anywhere in the deck).
    const int numShuffles = 50000;
    Deck deck;

    // Seed our random so we can be sure to get the same results every time we run this.
    Random random(1000);
    
    // Some of the 256 indices will not be used since we only have 52 different types of cards, but for speed
    // purposes, it would be nice to just index with the card_t value. This relies on the format of card_t being 
    // a single byte, so ensure that here.
    ASSERT_EQ(sizeof(card_t), sizeof(uint8_t));

    // half mB datbase mapping card index to the count of each card during the shuffle.
    std::array<std::array<int, 256>, NumCardsInDeck> numOccurrences;
    numOccurrences[0].fill(0);
    numOccurrences.fill(numOccurrences[0]);
    
    for (int i = 0; i < numShuffles; i++)
    {
        deck.ShuffleAndReset(Deck::OptimalShuffleCount, random);
        ValidateDeck(deck);
        
        card_t currentCard = deck.Draw();
        int deckIndex = 0;

        while (currentCard != NotACard)
        {
            numOccurrences[deckIndex][currentCard]++;
            currentCard = deck.Draw();
            deckIndex++;
        }
    }
    
    // After we finish shuffling n times and filling out the database, ensure we get sufficient randomness.
    // Since we shuffled many times, there should be a relatively equal chance of any card appearing in any deck position.
    // At 10000 shuffles, each card has a 1/52 chance to appear at any position.
    // 10000 * (1/52) is our expected number of occurrences at each position.
    // for the purposes of this test, we will use a high threshold, we just want to make sure we get each card in every position many times.
    constexpr int expectedNumOccurrences = static_cast<int>(((1.0 / NumCardsInDeck) * numShuffles));
    constexpr int numOccurrencesThreshold = 160;
    
    // Data for statistics computation on each card position.
    std::array<uint64_t, 256> sum;
    std::array<uint64_t, 256> sumSq;
    sum.fill(0);
    sumSq.fill(0);

    for (int i = 0; i < NumCardsInDeck; i++)
    {
        // Iterate through every card, and make sure it appeared in the deck the expected number of times.
        for (int rank = 0; rank < static_cast<int>(Rank::MaxRank); rank++)
        {
            for (int suit = 0; suit < static_cast<int>(Suit::MaxSuit); suit++)
            {
                const card_t card = Card::CreateCard(static_cast<Rank>(rank), static_cast<Suit>(suit));
                int nOccurrences = numOccurrences[i][card];
                
                sum[card] += nOccurrences;
                sumSq[card] += static_cast<uint64_t>(nOccurrences * nOccurrences);
                
                ASSERT_GT(nOccurrences, expectedNumOccurrences - numOccurrencesThreshold);
                ASSERT_LT(nOccurrences, expectedNumOccurrences + numOccurrencesThreshold);
            }
        }
    }
    
    // The means should be exactly the number of occurrences.
    for (int rank = 0; rank < static_cast<int>(Rank::MaxRank); rank++)
    {
        for (int suit = 0; suit < static_cast<int>(Suit::MaxSuit); suit++)
        {
            const card_t card = Card::CreateCard(static_cast<Rank>(rank), static_cast<Suit>(suit));
            const double mean = sum[card] / static_cast<double>(NumCardsInDeck);
            
            DoubleCompareEq(mean, expectedNumOccurrences, 0.01);
        }
    }
}

namespace
{
   void DoubleCompareEq(double d1, double d2, double thresh)
   {
      ASSERT_TRUE(d1 > d2 - (d2 * thresh) && d1 < d2 + (d2 * thresh));
   }

   /// @brief Ensure that each card appears in the deck, one time.
   void ValidateDeck(Deck& deck)
   {
      std::array<int, 256> db;
      db.fill(0);

      for (int rank = 0; rank < static_cast<int>(Rank::MaxRank); rank++)
      {
         for (int suit = 0; suit < static_cast<int>(Suit::MaxSuit); suit++)
         {
            const card_t card = Card::CreateCard(static_cast<Rank>(rank), static_cast<Suit>(suit));
            db[card] = 1;
         }
      }

      deck.Reset();

      card_t currentCard = deck.Draw();
      while (currentCard != NotACard)
      {
         db[currentCard]--;
         currentCard = deck.Draw();
      }

      deck.Reset();

      for (int i = 0; i < 256; i++)
      {
         if (db[i] != 0)
         {
            ASSERT_FALSE(true);
         }
      }
   }

}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}