/*
 * Neo6mTest.h
 *
 *  Created on: 01/05/2023
 *      Author: Utilizador
 */

#ifndef NEO6M_TESTS_H_
#define NEO6M_TESTS_H_

#include "Neo6mLiteFlex.h"

UT_STATIC Neo6mLiteFlexStatus_t IOReadIntoRingBuffer(Neo6mLiteFlex_t Neo6mLiteFlex, size_t CopySize);
UT_STATIC uint32_t GetBytesUntilSequenceEnd(lwrb_t* pRingBuf, char* Sequence);
UT_STATIC char GetCharBeforeSequence(lwrb_t* pRingBuf,char* Sequence);
UT_STATIC float GetFloatUntilSequence(lwrb_t* pRingBuf,char* Sequence);
UT_STATIC uint16_t GetIntUntilSequence(lwrb_t* pRingBuf,char* Sequence);
UT_STATIC uint16_t GetNextBytesAsInt(lwrb_t* pRingBuf,uint32_t BytesToRead);
UT_STATIC Neo6mDefaultMsg_t GetDefaultMsg(lwrb_t* pRingBuf);

#endif /* NEO6M_TESTS_H_ */
