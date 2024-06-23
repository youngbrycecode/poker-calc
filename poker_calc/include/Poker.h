#pragma once

#include <vector>
#include <memory>
#include <exception>

#include "Random.h"
#include "Deck.h"

namespace sim
{
   /// The clasifications (and rankings of poker hands)
   enum class HandClass
   {
      // Highest card in your hand.
      HighCard,
      // Two matching cards.
      OnePair,
      // Two matching cards of separate ranks.
      TwoPair,
      // Three cards of the same rank.
      ThreeOfAKind,
      // Five cards in a row with different suits.
      // Aces can be either used as the high or low card. (1/13)
      Straight,
      // Five cards of the same suit.
      Flush,
      // Three of a kind plus a pair.
      FullHouse,
      // Four cards of the same rank.
      FourOfAKind,
      // Five cards in a row with the same suit.
      StraightFlush,
      // Straight flush with 10-Ace
      RoyalFlush,
      MaxClassification
   };

#pragma region HAND_TYPE_STRUCTS

   struct tHighCardData
   {
      Rank HighRank;
   };

   struct tOnePairData
   {
      Rank HighRank;
   };

   struct tTwoPairData
   {
      Rank SecondPairRank;
   };

   struct tThreeOfAKindData
   {
      Rank HighRank;
   };

   struct tStraightData
   {
      Rank HighRank;
   };

   struct tFlushData
   {
      Suit FlushSuit;
   };

   struct tFourOfAKindData
   {
      Rank HighRank;
   };

   struct tStraightFlushData
   {
      Rank HighRank;
   };

   struct tClassificationData
   {
      HandClass HandClassification;

      // Classification data.
      tHighCardData HighCard;
      tOnePairData OnePair;
      tTwoPairData TwoPair;
      tThreeOfAKindData ThreeOfAKind;
      tStraightData Straight;
      tFlushData Flush;
      tFourOfAKindData FourOfAKind;
      tStraightFlushData StraightFlush;

      card_t HighestCard;
      card_t LowestCard;

      /// Bit fields computed during the preprocessing phase
      /// These store for each rank 4 bits for which suits exist
      /// And for each suit, 13 bits for each rank card that exists having that suit.
      uint32_t RankBitFields[static_cast<int>(Rank::MaxRank)];
      uint32_t SuitBitFields[static_cast<int>(Suit::MaxSuit)];

      // The number of cards at the current rank.
      uint32_t RankCardCount[static_cast<int>(Rank::MaxRank)];
      uint8_t SuitCardCount[static_cast<int>(Suit::MaxSuit)];


      /// @brief This rank bit field is independent of suit.
      /// A value is always set if the rank appears.
      uint32_t RankBitField;
      uint64_t CardsBitField;

      enum ClassTested : uint8_t
      {
         Fail,
         Pass
      };

      ClassTested ClassesTested[static_cast<int>(HandClass::MaxClassification)];
   };

#pragma endregion HAND_TYPE_STRUCTS

   class HandClassification
   {
   public:
      HandClassification() = default;

      /// Classify the cards. Creates classifications for 
      /// preflop, postflop, with turn, and with river.
      /// Use NotACard if you don't want to add the card to the hand.
      /// @param hand1 
      /// @param hand2 
      /// @param flop1 
      /// @param flop2 
      /// @param flop3 
      /// @param turn 
      /// @param river 
      void ClassifyAllCardsFast(card_t hand1, card_t hand2,
         card_t flop1, card_t flop2, card_t flop3,
         card_t turn, card_t river);

      void ClassifyArbitraryNumCards(card_t* cards, int numCards);

      void PrintAllCardsClassification(std::ostream& outputStream);
      tClassificationData& GetClassificationData() { return mAllCardsClassData; }

   private:
      tClassificationData mAllCardsClassData;
   };

   class PokerGameState
   {
   public:
      PokerGameState(int numPlayers, int seed = 100);
      virtual ~PokerGameState();

      /// Deal cards to every player.
      void Deal();

      /// Discard top card and reveal three cards (flop)
      void DealFlop();

      /// Burn a card, then reveal the next besides the flop.
      void DealTurn();

      /// Deal the last card on the table.
      void DealRiver();

      /// Shuffles the cards, and resets the game to the starting state.
      void Reset();

      /// Returns the classification data for the selected player.
      tClassificationData& GetPlayerClasification(int playerIndex)
      {
         if (playerIndex < mHands.size())
         {
            mHands[playerIndex].ClassifyHand(mFlop[0], mFlop[1], mFlop[2], NotACard, NotACard);
            return mHands[playerIndex].GetClassificationData();
         }
         else
         {
            throw std::exception("Player index out of range\n");
         }
      }

   private:
      /// The three cards played after deal.
      card_t mFlop[3];
      card_t mTurn;
      card_t mRiver;

      Deck mDeck;
      Random mRandom;

      /// Class to store information about a hand in poker.
      class Hand
      {
      public:
         Hand() = default;

         /// Puts a card in the hand.
         /// @param card 
         void DealCard(card_t card);

         /// Resets the hand for the next deal cycle.
         void ClearHand();

         void ClassifyHand(card_t flop1, card_t flop2, card_t flop3, card_t turn, card_t river)
         {
            mHandClassification.ClassifyAllCardsFast(mHand[0], mHand[1], flop1, flop2, flop3, NotACard, NotACard);
         }

         tClassificationData& GetClassificationData()
         {
            return mHandClassification.GetClassificationData();
         }

         static constexpr int HandSize = 2;

      private:
         card_t mHand[HandSize];
         int mHandIndex = 0;
         HandClassification mHandClassification;
      };

      std::vector<Hand> mHands;
   };
}