#include <iostream>
#include <fstream>

#include "LookupTables.h"
#include "Poker.h"

using namespace sim;

void PerformClassificationStats(uint64_t numTrials);

int main()
{
   InitLookupTables();

   const uint64_t numSims = 100e6;
   PerformClassificationStats(numSims);

   CleanupLookupTables();
   return 0;
}

void PerformClassificationStats(uint64_t numTrials)
{
   HandClassification classifier;
   Deck deck;
   Random random(101);

   uint64_t classificationCounts[static_cast<int>(HandClass::MaxClassification)] = { 0 };

   for (uint64_t i = 0; i < numTrials; i++) 
   {
      deck.ShuffleAndReset(Deck::OptimalShuffleCount, random);

      card_t hand1 = deck.Draw();
      card_t hand2 = deck.Draw();

      deck.Draw();

      card_t f1 = deck.Draw();
      card_t f2 = deck.Draw();
      card_t f3 = deck.Draw();

      deck.Draw();
      card_t turn = deck.Draw();

      deck.Draw();
      card_t river = deck.Draw();

      classifier.ClassifyAllCardsFast(hand1, hand2, f1, f2, f3, turn, river);
      HandClass classification = classifier.GetClassificationData().HandClassification;

      classificationCounts[static_cast<int>(classifier.GetClassificationData().HandClassification)]++;
   }

   for (int i = 0; i < static_cast<int>(HandClass::MaxClassification); i++)
   {
      double probability = (classificationCounts[i] / static_cast<double>(numTrials));
      std::cout << i << ": 1/" << 1.0 / probability << " : " << 100 * probability << "%\n";
   }
}