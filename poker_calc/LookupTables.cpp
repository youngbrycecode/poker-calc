#include "LookupTables.h"

#include <bitset>

#include "Cards.h"
#include "Poker.h"
using namespace sim;

void InitStraightFlushLookupTable()
{
   StraightFlushLookupTable = new uint8_t[1 << 21 << 0];

   card_t cards[20] = { 0 };
   HandClassification handClass;

   for (uint32_t i = 0; i < (1 << 20); i++)
   {
      // Create cards from bits in the number. Every bit is a card.
      for (uint32_t j = 0; j < 20; j++)
      {
         const uint32_t bit = (1 << j);

         if ((i & bit) != 0)
         {
            const uint32_t rank = j / 4;
            const uint32_t suit = j % 4;
            const card_t card = Card::CreateCard(static_cast<Rank>(rank), static_cast<Suit>(suit));

            cards[j] = card;
         }
         else
         {
            cards[j] = NotACard;
         }
      }

      handClass.ClassifyArbitraryNumCards(cards, 20);
      tClassificationData classif = handClass.GetClassificationData();

      if (classif.ClassesTested[static_cast<int>(HandClass::Straight)] == tClassificationData::Pass)
      {
         StraightFlushLookupTable[i] = static_cast<int>(HandClass::Straight);
      }
      if (classif.ClassesTested[static_cast<int>(HandClass::StraightFlush)] == tClassificationData::Pass)
      {
         StraightFlushLookupTable[i] = static_cast<int>(HandClass::StraightFlush);
      }
      else
      {
      }
   }
}

void InitRankCardCountLookupTable()
{
   RankCardCountLookupTable = new uint8_t[0x10];

   for (int i = 0; i < 0x10; i++)
   {
      RankCardCountLookupTable[i] = static_cast<uint8_t>(std::bitset<8>(i).count());
   }
}

void InitLookupTables()
{
   InitRankCardCountLookupTable();
   InitStraightFlushLookupTable();
}

void CleanupLookupTables()
{
   delete[] RankCardCountLookupTable;
   delete[] StraightFlushLookupTable;
}

