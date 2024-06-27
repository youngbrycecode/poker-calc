#include "Compare.h"
#include "LookupTables.h"

namespace sim
{
   int CompareHighCard(tClassificationData& first, tClassificationData& second)
   {
      if (first.HighCard.HighRank != second.HighCard.HighRank)
      {
         return static_cast<int>(first.HighCard.HighRank) - 
                static_cast<int>(second.HighCard.HighRank);
      }

      // See who has the highest high card.
      for (int i = static_cast<int>(Rank::Ace); i >= static_cast<int>(Rank::Two); i--)
      {
         const uint32_t bitShift = i << 2;
         const uint64_t mask = 0xFULL << bitShift;

         const int firstNCards = RankCardCountLookupTable[((first.CardsBitField & mask) >> bitShift) & 0xFULL];
         const int secondNCards = RankCardCountLookupTable[((second.CardsBitField & mask) >> bitShift) & 0xFULL];

         if (firstNCards != secondNCards)
         {
            return firstNCards - secondNCards;
         }
      }

      // They all had the same cards.
      return 0;
   }

   int CompareOnePair(tClassificationData& first, tClassificationData& second)
   {
      // See who has the highest pair.
      if (first.OnePair.HighRank != second.OnePair.HighRank)
      {
         return static_cast<int>(first.OnePair.HighRank) - 
                static_cast<int>(second.OnePair.HighRank);
      }

      // If tied, fallback to high card comparison.
      return CompareHighCard(first, second);
   }

   int CompareTwoPair(tClassificationData& first, tClassificationData& second)
   {
      // We happen to know that the highest rank is actually stored in the One pair rank because 
      // one pair is guaranteed to get filled in first based on the way classification works.
      if (first.OnePair.HighRank != second.OnePair.HighRank)
      {
         return static_cast<int>(first.TwoPair.SecondPairRank) - 
                static_cast<int>(second.TwoPair.SecondPairRank);
      }
      else if (first.TwoPair.SecondPairRank != second.TwoPair.SecondPairRank)
      {
         return static_cast<int>(first.TwoPair.SecondPairRank) - 
                static_cast<int>(second.TwoPair.SecondPairRank);
      }

      return CompareHighCard(first, second);
   }

   int CompareThreeOfAkind(tClassificationData& first, tClassificationData& second)
   {
      if (first.ThreeOfAKind.HighRank != second.ThreeOfAKind.HighRank)
      {
         return static_cast<int>(first.ThreeOfAKind.HighRank) - 
                static_cast<int>(second.ThreeOfAKind.HighRank);
      }

      return CompareHighCard(first, second);
   }

   int CompareStraight(tClassificationData& first, tClassificationData& second)
   {
      if (first.Straight.HighRank != second.Straight.HighRank)
      {
         return static_cast<int>(first.Straight.HighRank) - 
                static_cast<int>(second.Straight.HighRank);
      }

      // There is no fallback comparison. The higher straight always wins, otherwise its a tie. 
      // Hence a straight with an Ace is the nut straight.
      return 0;
   }

   int CompareFlush(tClassificationData& first, tClassificationData& second)
   {
      const int firstFlushSuitBits = static_cast<int>(first.Flush.FlushSuit);
      const int secondFlushSuitBits = static_cast<int>(second.Flush.FlushSuit);

      for (int i = static_cast<int>(Rank::Ace); i >= static_cast<int>(Rank::Two); i--)
      {
         // Find the highest cards which are part of the flush. Note that only one flush can 
         // exist per hand since two flushes would require 10 cards.
         uint64_t firstCardBit = (1ull << firstFlushSuitBits) << (i << 2);
         uint64_t secondCardBit = (1ull << secondFlushSuitBits) << (i << 2);

         int card1Exists = (first.CardsBitField & firstCardBit) != 0;
         int card2Exists = (second.CardsBitField & secondCardBit) != 0;

         // If the card wasn't matched in both hands' flush, return the one it did appear in.
         if (card1Exists != card2Exists)
         {
            return card1Exists ? 1 : -1;
         }
      }

      // If two players have the same flush, the hands are tied.
      return 0;
   }

   int CompareFullHouse(tClassificationData& first, tClassificationData& second)
   {
      // See who has the higher three of a kind.
      if (first.FullHouse.ThreeOfAKindRank != second.FullHouse.ThreeOfAKindRank)
      {
         return static_cast<int>(first.FullHouse.ThreeOfAKindRank) - 
                static_cast<int>(second.FullHouse.ThreeOfAKindRank);
      }

      // See who has the higher pair.
      if (first.FullHouse.PairRank != second.FullHouse.PairRank)
      {
         return static_cast<int>(first.FullHouse.PairRank) - 
                static_cast<int>(second.FullHouse.PairRank);
      }

      return 0;
   }

   int CompareFourOfAKind(tClassificationData& first, tClassificationData& second)
   {
      if (first.FourOfAKind.HighRank != second.FourOfAKind.HighRank)
      {
         return static_cast<int>(first.FourOfAKind.HighRank) - 
                static_cast<int>(second.FourOfAKind.HighRank);
      }

      return CompareHighCard(first, second);
   }

   int CompareStraightFlush(tClassificationData& first, tClassificationData& second)
   {
      if (first.StraightFlush.HighRank != second.StraightFlush.HighRank)
      {
         return static_cast<int>(first.StraightFlush.HighRank) - 
                static_cast<int>(second.StraightFlush.HighRank);
      }

      // There is no fallback comparison. The higher straight always wins, otherwise its a tie. 
      // Hence a straight with an Ace is the nut straight.
      return 0;
   }

   int CompareHands(tClassificationData& first, tClassificationData& second)
   {
      if (first.HandClassification == second.HandClassification)
      {
         switch (first.HandClassification)
         {
         case HandClass::HighCard:
            return CompareHighCard(first, second);
            break;
         case HandClass::OnePair:
            return CompareOnePair(first, second);
            break;
         case HandClass::TwoPair:
            return CompareTwoPair(first, second);
            break;
         case HandClass::ThreeOfAKind:
            return CompareThreeOfAkind(first, second);
            break;
         case HandClass::Straight:
            return CompareStraight(first, second);
            break;
         case HandClass::Flush:
            return CompareFlush(first, second);
            break;
         case HandClass::FullHouse:
            return CompareFullHouse(first, second);
            break;
         case HandClass::FourOfAKind:
            return CompareFourOfAKind(first, second);
            break;
         case HandClass::StraightFlush:
            return CompareStraightFlush(first, second);
            break;
         case HandClass::RoyalFlush:
            // If they both have royal flushes, the pot is split no matter what.
            return 0;
            break;
         }
      }

      // We know one hand is immediately better than the other.
      return static_cast<int>(first.HandClassification) - static_cast<int>(second.HandClassification);
   }
}
