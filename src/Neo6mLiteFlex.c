/*
 * Neo6m.c
 *
 *  Created on: May 1, 2023
 *      Author: Utilizador
 */
#include <stdint.h>
#include <stdlib.h>

#include "Neo6mLiteFlex.h"

typedef struct Neo6mLiteFlexStruct
{
	lwrb_t MessageRingBuffer;
	uint8_t MessageByteArray[NEEO6M_BATCH_BUFFER_SIZE];
} Neo6mLiteFlexStruct_t;

Neo6mLiteFlex_t Neo6mLiteFlex_Create()
{
	Neo6mLiteFlex_t Neo6mLiteFlex;
	Neo6mLiteFlex = (Neo6mLiteFlex_t) malloc(sizeof(Neo6mLiteFlexStruct_t));

	lwrb_init(&(Neo6mLiteFlex->MessageRingBuffer),Neo6mLiteFlex->MessageByteArray,NEEO6M_BATCH_BUFFER_SIZE);

	return Neo6mLiteFlex;

}

void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	free(Neo6mLiteFlex);
}


UT_STATIC Neo6mLiteFlexStatus_t CopyFromBufferToRingBuffer(Neo6mLiteFlex_t Neo6mLiteFlex, size_t CopySize)
{

}
