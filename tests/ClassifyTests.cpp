#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include "Deck.h"
#include "LookupTables.h"
#include "Poker.h"
#include "src/gtest-internal-inl.h"

using namespace sim;

namespace
{
   int LoadNextNumber(const char* string, size_t& index)
   {
      int prevIdx = index;
      const int value = std::stoi(string + index, &index);
      index += prevIdx + 1;
      return value;
   }

   card_t LoadNextCard(const char* string, size_t& index)
   {
      int s, r;

      s = LoadNextNumber(string, index);
      r = LoadNextNumber(string, index);

      // An ace is a 1 in the schema.
      if (r == 1)
      {
         r = static_cast<int>(Rank::Ace) + 2;
      }

      const Suit suit = static_cast<Suit>(s - 1);
      const Rank rank = static_cast<Rank>(r - 2);

      return Card::CreateCard(rank, suit);
   }

   HandClass LoadCardsAndClassification(const char* currentLine, card_t& hand1, card_t& hand2,
      card_t& flop1, card_t& flop2, card_t& flop3)
   {
      size_t index = 0;

      hand1 = LoadNextCard(currentLine, index);
      hand2 = LoadNextCard(currentLine, index);
      flop1 = LoadNextCard(currentLine, index);
      flop2 = LoadNextCard(currentLine, index);
      flop3 = LoadNextCard(currentLine, index);

      int classif = LoadNextNumber(currentLine, index);

      return static_cast<HandClass>(classif - 1);
   }

   /// <summary>
   /// Tests both the fast and slow classification interfaces to ensure they arrive at the same correct conclusion.
   /// </summary>
   /// <param name="hand1"></param>
   /// <param name="hand2"></param>
   /// <param name="flop1"></param>
   /// <param name="flop2"></param>
   /// <param name="flop3"></param>
   /// <param name="turn"></param>
   /// <param name="river"></param>
   /// <param name="expectedClassification"></param>
   void EnsureClassification(card_t hand1, card_t hand2, card_t flop1, card_t flop2, card_t flop3,
      card_t turn, card_t river, HandClass expectedClassification)
   {
      card_t hand[7];
      hand[0] = hand1;
      hand[1] = hand2;
      hand[2] = flop1;
      hand[3] = flop2;
      hand[4] = flop3;
      hand[5] = turn;
      hand[6] = river;

      HandClassification classification;
      classification.ClassifyAllCardsFast(hand1, hand2, flop1, flop2, flop3, turn, river);
      ASSERT_EQ(classification.GetClassificationData().HandClassification, expectedClassification);

      classification.ClassifyArbitraryNumCards(hand, 7);
      ASSERT_EQ(classification.GetClassificationData().HandClassification, expectedClassification);
   }
}

/// <summary>
/// Test an edge case I discovered where three of a kinds were not properly being converted to full houses.
/// A full house doesn't need to include strictly a pair, it can also include two three of a kinds, or even a four of a kind.
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(ClassifyTests, ThreeOfAKindToFullHouseConversions)
{
   // Standard fullhouse.
   card_t hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
   card_t hand2 = Card::CreateCard(Rank::Ace, Suit::Diamonds);
   card_t flop1 = Card::CreateCard(Rank::Ace, Suit::Spades);
   card_t flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
   card_t flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
   card_t turn = Card::CreateCard(Rank::Three, Suit::Hearts);
   card_t river = Card::CreateCard(Rank::Two, Suit::Hearts);
   EnsureClassification(hand1, hand2, flop1, flop2, flop3, turn, river, HandClass::FullHouse);

   // Full house made from two three of a kinds.
   hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
   hand2 = Card::CreateCard(Rank::Ace, Suit::Diamonds);
   flop1 = Card::CreateCard(Rank::Ace, Suit::Spades);
   flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
   flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
   turn = Card::CreateCard(Rank::Five, Suit::Hearts);
   river = Card::CreateCard(Rank::Two, Suit::Hearts);
   EnsureClassification(hand1, hand2, flop1, flop2, flop3, turn, river, HandClass::FullHouse);

   // Four of a kind is not converted to a full house, but has full house classificaton data.
   hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
   hand2 = Card::CreateCard(Rank::Two, Suit::Diamonds);
   flop1 = Card::CreateCard(Rank::Two, Suit::Spades);
   flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
   flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
   turn = Card::CreateCard(Rank::Five, Suit::Spades);
   river = Card::CreateCard(Rank::Five, Suit::Clubs);
   EnsureClassification(hand1, hand2, flop1, flop2, flop3, turn, river, HandClass::FourOfAKind);

   // Ensure we get full house data in the fast processor.
   HandClassification classification;
   classification.ClassifyAllCardsFast(hand1, hand2, flop1, flop2, flop3, turn, river);
   ASSERT_EQ(classification.GetClassificationData().ClassesTested[static_cast<int>(HandClass::FullHouse)], tClassificationData::Pass);

   hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
   hand2 = Card::CreateCard(Rank::Ace, Suit::Diamonds);
   flop1 = Card::CreateCard(Rank::Ace, Suit::Spades);
   flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
   flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
   turn = Card::CreateCard(Rank::Five, Suit::Spades);
   river = Card::CreateCard(Rank::Five, Suit::Clubs);
   EnsureClassification(hand1, hand2, flop1, flop2, flop3, turn, river, HandClass::FourOfAKind);

   // Ensure we get full house data in the fast processor.
   classification.ClassifyAllCardsFast(hand1, hand2, flop1, flop2, flop3, turn, river);
   ASSERT_EQ(classification.GetClassificationData().ClassesTested[static_cast<int>(HandClass::FullHouse)], tClassificationData::Pass);
}

/// This test just runs the classify function on a two pair many many times and reports how long it takes to do so.
TEST(ClassifyTests, ClassifySpeedTests)
{
   std::cout << "Executing classify performance test: \n";
   HandClassification classification;
   const card_t hand1 = Card::CreateCard(Rank::Ace, Suit::Hearts);
   const card_t hand2 = Card::CreateCard(Rank::Ace, Suit::Diamonds);
   const card_t flop1 = Card::CreateCard(Rank::Two, Suit::Spades);
   const card_t flop2 = Card::CreateCard(Rank::Five, Suit::Hearts);
   const card_t flop3 = Card::CreateCard(Rank::Five, Suit::Diamonds);
   const card_t turn = Card::CreateCard(Rank::Three, Suit::Hearts);
   const card_t river = Card::CreateCard(Rank::Two, Suit::Hearts);

   constexpr int numClassifies = 5e7;

   auto start = std::chrono::high_resolution_clock::now();
   for (int i = 0; i < numClassifies; i++)
   {
      classification.ClassifyAllCardsFast(hand1, hand2, flop1, flop2, flop3, turn, river);
   }
   auto finish = std::chrono::high_resolution_clock::now();

   unsigned long long totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
   double totalTimeSeconds = static_cast<double>(totalTime) / 1e9;

   std::cout << (static_cast<double>(numClassifies)) / totalTimeSeconds << " Classifications per second\n";
}

/// I was only able to find a large data set of five card poker hand classifications.
/// I decided to use that for some classification testing since it'll still get some good coverage.
/// I wont be able to test the classification data it produced here, so that will be another test.
/// Test data is not included in this git repo:
///  https://www.openml.org/search?type=data&sort=runs&id=1567&status=active
TEST(ClassifyTests, FiveCardPokerHandClassificationTest)
{
   // Load the dataset into an fstream.
   std::fstream dataSet("./php33Mc7a.arff", std::ios::in);
   std::string currentLine;

   ASSERT_TRUE(dataSet.is_open());

   while (std::getline(dataSet, currentLine))
   {
      if (currentLine[0] != '@' && currentLine.length() > 10) break;
   }

   HandClassification classifier;

   do
   {
      // If there's a new line, quit.
      if (currentLine.length() < 5) break;

      card_t hand1, hand2, flop1, flop2, flop3;
      HandClass expectedClassification = LoadCardsAndClassification(currentLine.c_str(), hand1, hand2, flop1, flop2, flop3);
      classifier.ClassifyAllCardsFast(hand1, hand2, flop1, flop2, flop3, NotACard, NotACard);

      HandClass classif = classifier.GetClassificationData().HandClassification;

      card_t cards[5];
      cards[0] = hand1;
      cards[1] = hand2;
      cards[2] = flop1;
      cards[3] = flop2;
      cards[4] = flop3;

      classifier.ClassifyArbitraryNumCards(cards, 5);
      HandClass classif2 = classifier.GetClassificationData().HandClassification;

      if (expectedClassification != classif ||
         expectedClassification != classif2)
      {
         Card::Print(hand1, std::cout);
         Card::Print(hand2, std::cout);
         Card::Print(flop1, std::cout);
         Card::Print(flop2, std::cout);
         Card::Print(flop3, std::cout);

         std::cout << "\n";
         classifier.PrintAllCardsClassification(std::cout);
         ASSERT_EQ(expectedClassification, classif);
         ASSERT_EQ(expectedClassification, classif2);
      }
   } while (std::getline(dataSet, currentLine));

   dataSet.close();
}

int main(int argc, char** argv)
{
   InitLookupTables();
   ::testing::InitGoogleTest(&argc, argv);

   const auto ret = RUN_ALL_TESTS();
   CleanupLookupTables();
   return ret;
}