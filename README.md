# NEO6M-Lite-Flex
  
When the NEO6M GPS module is in its factory settings it outputs a sequence of six  NMEA 0183 messages in ASCII (GPRMC+GPVTG+GPGGA,GPGSA,GPGSV,GPGLL). What this library is essentially provide a simple way to parse that data into a structure with numeric daat that can then be used for multiple purposes.
MPU6050LiteFlex is built with a focus on simplicity and flexibility, meaning it can be easily used on a variety of platforms such as Stm32, Arduino, Raspberry Pi, and other microcontrollers, and provides only simple functionality, as it merely parses the default messages that NEO-6M outputs.
NEO6M-Lite-Flex is written purely in C.
# Features
The key features that NEO6M-Lite-Flex provides are
 - Letting the client set platform specific IO functions with ease
 - Allows the instantiation of multiple NEO6M-Lite-Flex "objects", so multiple NEO-6M devices can be used by the same client.
 - Lets the client obtain the data that is automatically output by the device in ASCII in a numeric format

The key API is as shown:

    Neo6mLiteFlex_Create();
    ### Create instance of Neo6m "Object"
    Neo6mLiteFlex_Destroy();
    ### Destroy instance of Neo6m"Object"
    Neo6mLiteFlex_SetIORead();
    ### Sets the client defined function for reading from the NEO6M device
    Neo6mLiteFlex_GetNeo6mMsgs();
    ### Reads all NEO6M default messages (GPRMC+GPVTG+GPGGA,GPGSA,GPGSV,GPGLL) into a provided default message array

NEO6M-Lite-Flex was built just to test some concepts for asynchronously receiving UART data and as such there was no point in it covering the full features that the NEO6M module offers. As a result only parsig of default messages is provided. If this library gains any traction I will gladly add any missing features so that it can be used to expand it to cover the complete NEO6M functionality. Additionally, pull requests are welcome.
# Contents
This library contains the following files in the following folders

 - **src**
	 - Mpu6050Flex.c
	 - Mpu6050Flex.h
 - **tests**
	 - all_tests.c
	 - Neo6m_MockIO.c
	 - Neo6m_MockIO.h
	 - Neo6m_Runner.c
	 - Neo6m_Tests.c
	 - Neo6m_Privates.h
	 - Neo6m_TestDataSets.h

### src
The source files are where the library is actually implented and what you would import to your applications.
### tests
This library's development followed Test Driven Development using the Unity unit test harnesss - the test files are provided. The MockIO files define the mock used to easily test the hardware interface without depending and allowing one to control its output. In the Tests file the tests are defined. In Privates, private functions that need to be public during testing are declared and in test data sets, the ASCII and struct data sets used for the tests are defined.
# Usage
The usage of this library is simple, but please have in mind the following information: 

 - This library depends on the lwrb library by MaJerle, which you can easily find here on github.
 - When reading UART data, it is up to the user to define how it should read data in a way that allows for reading the minimum defined amount of bytes that guarantee that at least a full sequence of NMEA 0183 messages in a single call (750 by default and with a large margin). If using polling, this may take several seconds so unblocking methods are preferred (The suggested method is buffering in an ISR until a certain amount of bytes are received and then call `Neo6mLiteFlex_GetNeo6mMsgs` and have the IO function read from the buffer instead of directly from UART).

Considering that, in your code the following steps should be perfomed:

 - Setup both Neo6mLiteFlex and lwrb
 - Create an Neo6mLiteFlex instance using `Neo6mFlex_Create();`
 - Set your IO reading function with`Neo6mLiteFlex_SetIORead();`(See above list)
 - `Call  Neo6mLiteFlex_GetNeo6mMsgs` and pass a `Neo6mMsgArray_t` variable for it fill in with the read data

## Limitations
Limitations of this library are regarding the lack of ability to use most of the NEO-6M features and only parsing the data output by default. 
Additionally, the read structue contains redundant data (as does the messages that are output by NEO6M), but a more compact version could be provided.
## To-do
 - Increase features to cover more of what NEO-6M offers
## Contributing
Pull requests and any sort of feedback/requests are welcome.
## Contact
You can reach me at hpontes9@gmail.com