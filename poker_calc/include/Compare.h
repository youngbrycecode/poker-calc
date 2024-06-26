#pragma once

#include "Poker.h"

namespace sim 
{
   /// <summary>
   /// Compares all aspects of two hands. Returns the better hand.
   /// </summary>
   /// <param name="first"></param>
   /// <param name="second"></param>
   /// <returns></returns>
   int CompareHands(tClassificationData& first, tClassificationData& second);
}
