#include "LookupTables.h"

#include "Cards.h"
#include "Poker.h"
using namespace sim;

// TODO: have the lookup table cache to a file if it doesn't already exit and load from that file if it does.
void InitStraightFlushLookupTable()
{
   StraightFlushLookupTable = new uint8_t[1 << 20];

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
      HandClass classif = handClass.GetClassification();

      if (classif == HandClass::Straight)
      {
         StraightFlushLookupTable[i] = static_cast<int>(HandClass::Straight);
      }
      else if (classif == HandClass::StraightFlush)
      {
         StraightFlushLookupTable[i] = static_cast<int>(HandClass::StraightFlush);
      }
      else
      {
      }
   }
}

void InitLookupTables()
{
   InitStraightFlushLookupTable();
}

