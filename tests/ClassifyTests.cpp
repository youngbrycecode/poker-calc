#include <gtest/gtest.h>
#include <fstream>
#include <chrono>
#include "Deck.h"
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
}

/// @brief This test just runs the classify function on a two pair many many times and reports how long it takes to do so.
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

   constexpr int numClassifies = 2e7;

   auto start = std::chrono::high_resolution_clock::now();
   for (int i = 0; i < numClassifies; i++)
   {
      classification.ClassifyAllCards(hand1, hand2, flop1, flop2, flop3, turn, river);
   }
   auto finish = std::chrono::high_resolution_clock::now();

   unsigned long long totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
   double totalTimeSeconds = static_cast<double>(totalTime) / 1e9;

   std::cout << (static_cast<double>(numClassifies)) / totalTimeSeconds << " Classifications per second\n";
}

/// @brief I was only able to find a large data set of five card poker hand classifications.
/// I decided to use that for some classification testing since it'll still get some good coverage.
/// I wont be able to test the classification data it produced here, so that will be another test.
/// Test data is not included in this git repo: https://www.openml.org/search?type=data&sort=runs&id=1567&status=active
TEST(ClassifyTests, FiveCardPokerHandClassificationTest)
{
   system("dir");
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
      classifier.ClassifyAllCards(hand1, hand2, flop1, flop2, flop3, NotACard, NotACard);

      if (expectedClassification != classifier.GetClassification())
      {
         Card::Print(hand1, std::cout);
         Card::Print(hand2, std::cout);
         Card::Print(flop1, std::cout);
         Card::Print(flop2, std::cout);
         Card::Print(flop3, std::cout);

         std::cout << "\n";
         classifier.PrintAllCardsClassification(std::cout);
         ASSERT_EQ(expectedClassification, classifier.GetClassification());
      }
   } while (std::getline(dataSet, currentLine));

   dataSet.close();
}

int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}