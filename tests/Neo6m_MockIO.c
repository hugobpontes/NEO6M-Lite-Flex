/*
 * Mpu6050_Mock.c
 *
 *  Created on: 10/04/2023
 *      Author: Utilizador
 */

#include <stdint.h>
#include "unity_fixture.h"
#include <string.h>
#include "Neo6mLiteFlex.h"

#define MAX_DATA_SIZE 1600 //TODO: CHANGE THIS,use define in neo6m.h file!!!

typedef enum
{
	READ_ID,
	GETMS_ID,
} MockFuncId_t;

typedef struct Expectations
{
  uint32_t 	FuncId;
  size_t 	ReadInputDataSize;
  void *  	ReadInputDataPtr;
  uint8_t 	ReadOutputData[MAX_DATA_SIZE];
  size_t 	ReadOutputDataSize;
  uint32_t 	GetMsOutputMs;
} Expectations_t;

static Expectations_t* ExpectationsArray = 0;
static Expectations_t CurrentFunctionCall;
static uint32_t SetExpectationsIdx;
static uint32_t GetExpectationsIdx;
static uint32_t MaxExpectationsIdx;


static void FailIfExpectationsArrayNotInitialized()
{
	if (!ExpectationsArray)
	{
		FAIL("Expectations Array Not Initialized");
	}
}
static void FailIfNoUnusedExpectations()
{
	if (GetExpectationsIdx >= SetExpectationsIdx)
	{
		FAIL("No Unused Expectations");
	}
}

static void FailIfNoRoomForExpectations()
{
	if (SetExpectationsIdx >= MaxExpectationsIdx)
	{
		FAIL("No Room for Expectations");
	}
}

static void FailIfUnexpectedSize()
{
	if (CurrentFunctionCall.ReadInputDataSize != ExpectationsArray[GetExpectationsIdx].ReadInputDataSize)
	{
		FAIL("Unexpected Data Size argument");
	}
}

static void FailIfInvalidPtr()
{
	if (!CurrentFunctionCall.ReadInputDataPtr)
	{
		FAIL("Invalid Data Pointer argument");
	}
}


static void FailIfUnexpectedFunc()
{
	if (CurrentFunctionCall.FuncId != ExpectationsArray[GetExpectationsIdx].FuncId)
	{
		FAIL("Unexpected function call");
	}
}
static void FailIfNotAllExpectationsUsed()
{
	if (GetExpectationsIdx < SetExpectationsIdx)
	{
		FAIL("Not all Expectations Used");
	}
}


void MockNeo6m_Create (uint32_t MaxExpectations)
{
	ExpectationsArray = calloc(MaxExpectations,sizeof(Expectations_t));

	SetExpectationsIdx = 0;
	GetExpectationsIdx = 0;
	MaxExpectationsIdx = MaxExpectations;
}
void MockNeo6m_Destroy()
{
	if (ExpectationsArray)
	{
		free(ExpectationsArray);
	}
	ExpectationsArray = 0;
}

static void MockStandardFailures()
{
	FailIfExpectationsArrayNotInitialized();
	FailIfNoUnusedExpectations();
	FailIfUnexpectedFunc();
}

void MockNeo6m_ExpectReadAndReturn(size_t InputReadSize,
								   void* OutputDataPtr, size_t OutputReadSize)
{
	uint8_t idx;

	FailIfExpectationsArrayNotInitialized();
	FailIfNoRoomForExpectations();
	ExpectationsArray[SetExpectationsIdx].FuncId = READ_ID;
	//Set input expectations
	ExpectationsArray[SetExpectationsIdx].ReadInputDataSize = InputReadSize;
	//Set return data
	ExpectationsArray[SetExpectationsIdx].ReadOutputDataSize = OutputReadSize;



	memcpy(ExpectationsArray[SetExpectationsIdx].ReadOutputData,OutputDataPtr,OutputReadSize);
	SetExpectationsIdx++;
}

void MockNeo6m_ExpectGetMsAndReturn(uint32_t Return)
{
	FailIfExpectationsArrayNotInitialized();
	FailIfNoRoomForExpectations();
	ExpectationsArray[SetExpectationsIdx].FuncId = GETMS_ID;
	ExpectationsArray[SetExpectationsIdx].GetMsOutputMs = Return;

	SetExpectationsIdx++;
}

uint32_t MockNeo6m_GetMs()
{
	CurrentFunctionCall.FuncId = GETMS_ID;
	MockStandardFailures();

	GetExpectationsIdx++;

	return ExpectationsArray[GetExpectationsIdx-1].GetMsOutputMs;
}

size_t MockNeo6m_Read(void* DataPtr, size_t DataSize)
{
	uint8_t idx;

	CurrentFunctionCall.FuncId = READ_ID;
	CurrentFunctionCall.ReadInputDataPtr = DataPtr;
	CurrentFunctionCall.ReadInputDataSize = DataSize;

	MockStandardFailures();

	FailIfUnexpectedSize();
	FailIfInvalidPtr();

	if (DataPtr)
	{
		memcpy(	DataPtr,
				ExpectationsArray[GetExpectationsIdx].ReadOutputData,
				ExpectationsArray[GetExpectationsIdx].ReadOutputDataSize);
	}
	GetExpectationsIdx++;

	return ExpectationsArray[GetExpectationsIdx-1].ReadOutputDataSize;
}

void MockNeo6m_VerifyComplete()
{
	FailIfNotAllExpectationsUsed();
}
