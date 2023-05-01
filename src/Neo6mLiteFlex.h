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
} Neo6mLiteFlexStatus_t;

#define NEO6M_BATCH_SIZE (750)
#define NEO6M_BUFFER_SAFETY_MARGIN (100)
#define NEEO6M_BATCH_BUFFER_SIZE NEO6M_BATCH_SIZE+NEO6M_BUFFER_SAFETY_MARGIN

typedef struct Neo6mLiteFlexStruct* Neo6mLiteFlex_t;

Neo6mLiteFlex_t Neo6mLiteFlex_Create();
void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex);


#endif /* NEO6MLITEFLEX_H_ */
