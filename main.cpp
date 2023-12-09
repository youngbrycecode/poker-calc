#include <iostream>
#include <fstream>

#include "LookupTables.h"
#include "Poker.h"

using namespace sim;

int main()
{
    InitLookupTables();

    const int numSims = 500e6;
    int count = 0;

    HandClassification classifier;
    Deck deck;
    Random random(101);

    uint64_t classificationCounts[static_cast<int>(HandClass::MaxClassification)] = { 0 };

    do 
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
        classificationCounts[static_cast<int>(classifier.GetClassificationData().HandClassification)]++;

        // Get the classification for the first player and create the table of classification statistics.
        count++;
    } while (count < numSims);

    for (int i = 0; i < static_cast<int>(HandClass::MaxClassification); i++)
    {
        double percent = (classificationCounts[i] / static_cast<double>(numSims));
        std::cout << i << ": 1/" << 1.0 / percent << " : " << percent << "%\n";
    }

    CleanupLookupTables();
    return 0;
}