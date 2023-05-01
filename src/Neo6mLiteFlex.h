/*
 * Neo6m.h
 *
 *  Created on: May 1, 2023
 *      Author: Utilizador
 */

#ifndef NEO6MLITEFLEX_H_
#define NEO6MLITEFLEX_H_

#include "lwrb.h"

#ifdef UNIT_TEST
  #define UT_STATIC
#else
  #define UT_STATIC static
#endif

typedef enum
{
	NEO6M_SUCCESS,
	NEO6M_BUFFER_OVERFLOW,
	NEO6M_NULL_PTR,
} Neo6mLiteFlexStatus_t;

typedef size_t (*IOFunc_t)(void*,size_t);

#define NEO6M_BATCH_SIZE (750)
#define NEO6M_BUFFER_SAFETY_MARGIN (100)
#define NEEO6M_BATCH_BUFFER_SIZE NEO6M_BATCH_SIZE+NEO6M_BUFFER_SAFETY_MARGIN

typedef struct Neo6mLiteFlexStruct* Neo6mLiteFlex_t;

Neo6mLiteFlex_t Neo6mLiteFlex_Create();
void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex);
void Neo6mLiteFlex_SetIORead(Neo6mLiteFlex_t Neo6mLiteFlex, IOFunc_t pIORead);
lwrb_t* Neo6mLiteFlex_GetRingBuffPtr(Neo6mLiteFlex_t Neo6mLiteFlex);
uint8_t* Neo6mLiteFlex_GetByteArray(Neo6mLiteFlex_t Neo6mLiteFlex);


#endif /* NEO6MLITEFLEX_H_ */
