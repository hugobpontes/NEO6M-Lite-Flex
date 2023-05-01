#ifndef NEO6M_MOCKIO_H_
#define NEO6M_MOCKIO_H_

#include <stddef.h>
#include <stdint.h>

void MockNeo6m_Create (uint32_t MaxExpectations);
void MockNeo6m_Destroy();
void MockNeo6m_ExpectReadAndReturn(size_t InputReadSize,
								   void* OutputDataPtr, size_t OutputReadSize);
void MockNeo6m_ExpectGetMsAndReturn(uint32_t Return);
uint32_t MockNeo6m_GetMs();
size_t MockNeo6m_Read(void* DataPtr, size_t DataSize);
void MockNeo6m_VerifyComplete();

#endif /* NEO6M_MOCKIO_H_ */
