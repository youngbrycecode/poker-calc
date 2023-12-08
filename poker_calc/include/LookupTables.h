#pragma once

#include "stdint.h"

inline uint8_t* StraightFlushLookupTable;
inline uint8_t* RankCardCountLookupTable;

void InitLookupTables();
void CleanupLookupTables();