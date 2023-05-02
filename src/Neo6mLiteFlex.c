/*
 * Neo6m.c
 *
 *  Created on: May 1, 2023
 *      Author: Utilizador
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Neo6mLiteFlex.h"

#define MAX_SEQUENCE_SIZE 10

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

typedef struct Neo6mLiteFlexStruct
{
	lwrb_t MessageRingBuffer;
	uint8_t MessageByteArray[NEO6M_BATCH_BUFFER_SIZE];
	IOFunc_t pIORead;
} Neo6mLiteFlexStruct_t;

void Neo6mLiteFlex_SetIORead(Neo6mLiteFlex_t Neo6mLiteFlex, IOFunc_t pIORead)
{
	Neo6mLiteFlex->pIORead=pIORead;
}

lwrb_t* Neo6mLiteFlex_GetRingBuffPtr(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	return &(Neo6mLiteFlex->MessageRingBuffer);
}

uint8_t* Neo6mLiteFlex_GetByteArray(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	return Neo6mLiteFlex->MessageByteArray;
}

Neo6mLiteFlex_t Neo6mLiteFlex_Create()
{
	Neo6mLiteFlex_t Neo6mLiteFlex;
	Neo6mLiteFlex = (Neo6mLiteFlex_t) malloc(sizeof(Neo6mLiteFlexStruct_t));

	lwrb_init(&(Neo6mLiteFlex->MessageRingBuffer),Neo6mLiteFlex->MessageByteArray,NEO6M_BATCH_BUFFER_SIZE);
	Neo6mLiteFlex->pIORead = NULL;

	return Neo6mLiteFlex;

}

void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	free(Neo6mLiteFlex);
}


UT_STATIC Neo6mLiteFlexStatus_t IOReadIntoRingBuffer(Neo6mLiteFlex_t Neo6mLiteFlex, size_t CopySize)
{
	Neo6mLiteFlexStatus_t Status = NEO6M_SUCCESS;

	lwrb_t* pRingBuf;
	uint8_t* ByteArray;
	size_t ReadBytes = 0;
	size_t FirstReadSize;
	size_t SecondReadSize;
	size_t FreeBytes;

	pRingBuf = &(Neo6mLiteFlex->MessageRingBuffer);
	ByteArray = Neo6mLiteFlex->MessageByteArray;
	FreeBytes = lwrb_get_free(pRingBuf);

	if (Neo6mLiteFlex->pIORead)
	{
		if(CopySize < FreeBytes)
		{
			/*Since buffer is circular and we are doing linear reads, we may need to do two separate reads*/
			FirstReadSize = min(lwrb_get_linear_block_write_length(pRingBuf),CopySize);
			SecondReadSize = CopySize-FirstReadSize;

			ReadBytes += Neo6mLiteFlex->pIORead(ByteArray+pRingBuf->w,FirstReadSize);
			lwrb_advance(pRingBuf, FirstReadSize);
			if (SecondReadSize)
			{
				ReadBytes += Neo6mLiteFlex->pIORead(ByteArray+pRingBuf->w,SecondReadSize);
				lwrb_advance(pRingBuf, SecondReadSize);
			}

			if (ReadBytes != CopySize)
			{
				Status = NEO6M_IO_ERROR;
			}
		}
		else
		{
			Status = NEO6M_BUFFER_OVERFLOW;
		}

	}
	else
	{
		Status = NEO6M_NULL_PTR;
	}

	return Status;
}

UT_STATIC uint32_t GetBytesUntilSequence(lwrb_t* pRingBuf, char* Sequence)
{
	uint32_t BytesUntilSeq = SEQUENCE_NOT_FOUND;
	uint32_t PositionChecked = 0;
	char SequenceBuffer[MAX_SEQUENCE_SIZE];
	uint32_t SequenceLength = 0;
	uint32_t FullBytes = 0;

	if (Sequence)
	{
		SequenceLength = strlen(Sequence);
	}
	if (SequenceLength && (SequenceLength<MAX_SEQUENCE_SIZE) && pRingBuf )
	{
		FullBytes = lwrb_get_full(pRingBuf);
		/*Peek bytes equal to the size of the fed sequence until a match is found*/
		while (PositionChecked<FullBytes)
		{
			lwrb_peek(pRingBuf,PositionChecked,SequenceBuffer,SequenceLength);
			SequenceBuffer[SequenceLength]='\0'; /*Make char buffer a string */
			if (!strcmp(SequenceBuffer,Sequence))
			{
				BytesUntilSeq = PositionChecked;
				break;
			}
			PositionChecked++;
		}

	}

	return BytesUntilSeq;
}
