#include <gtest/gtest.h>
#include <fstream>
#include <chrono>

#include "Compare.h"
#include "Deck.h"
#include "LookupTables.h"
#include "Poker.h"
#include "src/gtest-internal-inl.h"

using namespace sim;

/// <summary>
/// Deals a hand of 5 cards from the deck.
/// </summary>
void RandomHand(Deck& currentDeck, card_t& hand1, card_t& hand2)
{
   hand1 = currentDeck.Draw();
   hand2 = currentDeck.Draw();
}

// Test to make sure that if one hand is classified higher than the other, it is also ranked better in all cases.
TEST(CompareTests, CompareHandClassifications)
{
   Random random(100);
   HandClassification player1Classification;
   HandClassification player2Classification;
   constexpr int numCompares = 1000;
   int compareCount = 0;

   Deck deck;
   for (int i = 0; i < numCompares; i++)
   {
      deck.ShuffleAndReset(Deck::OptimalShuffleCount, random);
      card_t player1Hand1 = deck.Draw();
      card_t player1Hand2 = deck.Draw();
      card_t player2Hand1 = deck.Draw();
      card_t player2Hand2 = deck.Draw();

      card_t flop1 = deck.Draw();
      card_t flop2 = deck.Draw();
      card_t flop3 = deck.Draw();
      card_t turn = deck.Draw();
      card_t river = deck.Draw();

      // Deal the flop, turn and river.
      player1Classification.ClassifyAllCardsFast(player1Hand1, player1Hand2, flop1, flop2, flop3, turn, river);
      player2Classification.ClassifyAllCardsFast(player2Hand1, player2Hand2, flop1, flop2, flop3, turn, river);

      if (player1Classification.GetClassificationData().HandClassification != player2Classification.GetClassificationData().HandClassification)
      {
         compareCount++;
         if (player1Classification.GetClassificationData().HandClassification > player2Classification.GetClassificationData().HandClassification)
         {
            ASSERT_GE(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
         }
         else 
         {
            ASSERT_GE(CompareHands(player2Classification.GetClassificationData(), player1Classification.GetClassificationData()), 0);
         }
      }
   }

   // Make sure this test actually had value. Our random hands could in theory deal equal hand strengths a lot. 
   // If that's the case, we need more trials, or need to change the seed.
   ASSERT_GE(compareCount, 500);
}

TEST(CompareTests, TestTieBreakerRoyalFlush)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have an identical Royal Flush, expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Clubs),
      Card::CreateCard(Rank::Five, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have a Royal Flush in different suits, expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Clubs)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has a Royal Flush, Player 2 does not, Player 1 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have potential Royal Flush components, but Player 1 completes the Royal Flush.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Hearts)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts)
   );
   ASSERT_LT(CompareHands(player2Classification.GetClassificationData(), player1Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerStraightFlush)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have an identical Straight Flush, expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Clubs),
      Card::CreateCard(Rank::Five, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with a higher top card (9 vs. 8).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Diamonds),
      Card::CreateCard(Rank::Three, Suit::Clubs)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Spades),
      Card::CreateCard(Rank::Six, Suit::Spades),
      Card::CreateCard(Rank::Five, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has a Straight Flush, Player 2 does not, Player 1 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Spades),
      Card::CreateCard(Rank::Six, Suit::Spades),
      Card::CreateCard(Rank::Five, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has a Wheel Straight Flush (5 high), Player 2 has a higher Straight Flush (9 high), Player 2 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Five, Suit::Clubs),
      Card::CreateCard(Rank::Four, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Eight, Suit::Hearts)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Spades),
      Card::CreateCard(Rank::Six, Suit::Spades),
      Card::CreateCard(Rank::Five, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have potential Straight Flush components, but Player 1 completes the higher Straight Flush.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Hearts)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerFourOfAKind)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have an identical Four of a Kind (Four Aces), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Four of a Kind (Aces vs. Kings).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with a higher kicker (King vs. Queen).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has Four of a Kind, Player 2 has a Full House, Player 1 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Four of a Kind (Threes vs. Twos).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs)
   );
   ASSERT_LT(CompareHands(player2Classification.GetClassificationData(), player1Classification.GetClassificationData()), 0);

   // Both players have the same Four of a Kind with an extra Three of a Kind, expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player2Classification.GetClassificationData(), player1Classification.GetClassificationData()), 0);

   // Player 1 wins with a higher kicker for the Four of a Kind (Ace vs. King).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Diamonds),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has Four of a Kind (Aces), Player 2 has a Full House with Aces over Kings, Player 1 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Spades)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 has higher Four of a Kind (Aces vs. Kings) but a lower kicker, Player 1 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Clubs)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs)
   );
   ASSERT_LT(CompareHands(player2Classification.GetClassificationData(), player1Classification.GetClassificationData()), 0);

   // Player 2 wins with higher Four of a Kind (Fours vs. Threes) and same kicker (Ace).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Four, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Clubs),
      Card::CreateCard(Rank::Four, Suit::Diamonds),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Clubs)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerFullHouse)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have an identical Full House (Aces over Kings), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Full House (Aces over Kings vs. Kings over Queens).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher pair in Full House (Aces over Kings vs. Aces over Queens).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Ensure higher Three of a Kind is selected (Player 1 wins with Aces over Kings vs. Queens over Jacks).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Ensure higher pair is selected (Player 1 wins with Kings over Aces vs. Aces over Kings).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player2 has two three of a kinds. The higher should be selected for the three of a kind.
   // Ace beats kings, player 2 wins.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Diamonds)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Full House (Queens over Jacks vs. Tens over Nines).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 2. They both have two pair, but player 2 has a 2 pair of aces.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Diamonds)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have equal pairs, player 2 has the lower 2 pair, but full houses are tied here.
   // Expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerFlush)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have identical Flushes (Ace-high), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher flush (Ace-high vs. King-high).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Clubs)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in flush (Ace-high with King kicker vs. Ace-high with Queen kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Clubs)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher flush (Seven-high vs. Six-high).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Six, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Diamonds),
      Card::CreateCard(Rank::Five, Suit::Clubs)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher flush (Nine-high vs. Eight-high).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Diamonds),
      Card::CreateCard(Rank::Three, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Hearts),
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Hearts),
      Card::CreateCard(Rank::Three, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Equality, equal flushes, player2 has the higher 6th card.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Seven, Suit::Clubs)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerStraight)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Player 1 wins with higher Straight (Ace high: Ace, King, Queen, Jack, 10) vs. lower Straight (King high: King, Queen, Jack, 10, 9).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Diamonds),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );

   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Straight (Jack high: Jack, 10, 9, 8, 7) vs. lower Straight (Ace low: 5, 4, 3, 2, Ace).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Six, Suit::Clubs),
      Card::CreateCard(Rank::Five, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Five, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have identical Straight (10 high: 10, 9, 8, 7, 6), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Spades),
      Card::CreateCard(Rank::Six, Suit::Diamonds),
      Card::CreateCard(Rank::Five, Suit::Clubs),
      Card::CreateCard(Rank::Four, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ten, Suit::Hearts),
      Card::CreateCard(Rank::Nine, Suit::Spades),
      Card::CreateCard(Rank::Eight, Suit::Hearts),
      Card::CreateCard(Rank::Seven, Suit::Spades),
      Card::CreateCard(Rank::Six, Suit::Diamonds),
      Card::CreateCard(Rank::Five, Suit::Clubs),
      Card::CreateCard(Rank::Four, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with Straight (9 high: 9, 8, 7, 6, 5) under Straight (2, 3, 4, 5, 6, 7).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Nine, Suit::Hearts),
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Spades),
      Card::CreateCard(Rank::Five, Suit::Diamonds),
      Card::CreateCard(Rank::Four, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Hearts),
      Card::CreateCard(Rank::Six, Suit::Spades),
      Card::CreateCard(Rank::Four, Suit::Diamonds),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerThreeOfAKind)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have identical Three of a Kinds (Aces with King kicker), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Three of a Kind (Aces vs. Kings).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with Three of a Kind (Aces) over Two Pair (Kings and Queens).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in Three of a Kind (Aces with King kicker vs. Aces with Queen kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Clubs),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Three of a Kind (Queens vs. Jacks).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Hearts),
      Card::CreateCard(Rank::Four, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in Three of a Kind (Queens with Ace kicker vs. Queens with King kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Clubs),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Three, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerTwoPair)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have identical Two Pairs (Aces and Kings with Queen kicker), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );

   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Two Pair (Aces and Kings vs. Kings and Queens).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Ten, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Two Pair (Aces and Kings vs. Kings and Queens).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with Two Pair (Aces and Kings) over One Pair (Aces).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in Two Pair (Aces and Kings with Queen kicker vs. Aces and Kings with Jack kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Diamonds),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher Two Pair (Kings and Queens vs. Queens and Jacks).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Jack, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerPair)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have identical One Pair (Aces with King, Queen, Jack kickers), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher One Pair (Aces with King kicker vs. Aces with Jack kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Diamonds),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in One Pair (Aces with King kicker vs. Aces with Queen kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Ace, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ten, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher One Pair (Kings with Queen kicker vs. Queens with Jack kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Nine, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher kicker in One Pair (Queens with Ace kicker vs. Queens with King kicker).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Ace, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::King, Suit::Diamonds),
      Card::CreateCard(Rank::Two, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

TEST(CompareTests, TestTieBreakerHighCard)
{
   HandClassification player1Classification;
   HandClassification player2Classification;

   // Both players have identical High Card (Ace), expect equality.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_EQ(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 1 wins with higher High Card (Ace vs. King).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Nine, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::King, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Two, Suit::Spades),
      Card::CreateCard(Rank::Nine, Suit::Diamonds),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );
   ASSERT_GT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Player 2 wins with higher top card (Ace vs. King), lower cards differ (Queen vs. Jack).
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Spades),
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Ten, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Eight, Suit::Clubs),
      Card::CreateCard(Rank::Seven, Suit::Diamonds)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Ace, Suit::Hearts),
      Card::CreateCard(Rank::King, Suit::Spades),
      Card::CreateCard(Rank::Queen, Suit::Hearts),
      Card::CreateCard(Rank::Jack, Suit::Spades),
      Card::CreateCard(Rank::Two, Suit::Diamonds),
      Card::CreateCard(Rank::Three, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Diamonds)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);

   // Both players have same cards all the way down to the last one.
   player1Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Six, Suit::Clubs),
      Card::CreateCard(Rank::Two, Suit::Spades)
   );

   player2Classification.ClassifyAllCardsFast(
      Card::CreateCard(Rank::Jack, Suit::Hearts),
      Card::CreateCard(Rank::Queen, Suit::Diamonds),
      Card::CreateCard(Rank::Ten, Suit::Clubs),
      Card::CreateCard(Rank::Eight, Suit::Spades),
      Card::CreateCard(Rank::Seven, Suit::Diamonds),
      Card::CreateCard(Rank::Six, Suit::Clubs),
      Card::CreateCard(Rank::Four, Suit::Spades)
   );
   ASSERT_LT(CompareHands(player1Classification.GetClassificationData(), player2Classification.GetClassificationData()), 0);
}

int main(int argc, char** argv)
{
   InitLookupTables();
   ::testing::InitGoogleTest(&argc, argv);

   const auto ret = RUN_ALL_TESTS();
   CleanupLookupTables();
   return ret;
}