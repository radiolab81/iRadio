#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <math.h>
#include "PAJ7620U2.h"

#define ISR_PIN 7

static int fd;

static int iRadioOn = TRUE;

/******************* PAJ7620U2 Driver Interface *****************************/
char I2C_readByte(int reg)
{
	return wiringPiI2CReadReg8(fd, reg);
}

unsigned short I2C_readU16(int reg)
{
	return wiringPiI2CReadReg16(fd, reg);
}
void I2C_writeByte(int reg, int val)
{
	wiringPiI2CWriteReg8(fd, reg, val);
}
unsigned char PAJ7620U2_init()
{
	unsigned char i,State;
	fd=wiringPiI2CSetup(PAJ7620U2_I2C_ADDRESS);
	delay(5);
	State = I2C_readByte(0x00);												//Read State
	if (State != 0x20) return 0;											//Wake up failed
	I2C_writeByte(PAJ_BANK_SELECT, 0);										//Select Bank 0
	for (i=0;i< Init_Array;i++)
	{
		I2C_writeByte(Init_Register_Array[i][0], Init_Register_Array[i][1]);//Power up initialize
	}
	return 1;
}

int main(int argc, char** argv)
{
	unsigned char i;
	unsigned short  Gesture_Data;
	printf("\nGesture Sensor  Deamon ...\n");

	if (wiringPiSetup() < 0) return 1;
	delay(5);
	if(!PAJ7620U2_init())
	{	printf("\nGesture Sensor Error\n");
		return 0;
	}

	printf("\nGesture Sensor OK\n");

	I2C_writeByte(PAJ_BANK_SELECT, 0);//Select Bank 0

	for (i = 0; i < Gesture_Array_SIZE; i++)
	{
		I2C_writeByte(Init_Gesture_Array[i][0], Init_Gesture_Array[i][1]);//Gesture register initializes
	}


	while(1) {
		Gesture_Data = I2C_readU16(PAJ_INT_FLAG1);

		if (Gesture_Data)
		{
			switch (Gesture_Data)
			{
				case PAJ_UP:		printf("Up\r\n");
									system("echo \"volup 2\" | nc 127.0.0.1 9294 -N");
				break;

				case PAJ_DOWN:		printf("Down\r\n");
									system("echo \"voldown 2\" | nc 127.0.0.1 9294 -N");
				break;

				case PAJ_LEFT:		printf("Left\r\n");
									system("echo \"prev\" | nc 127.0.0.1 9294 -N");
				break;

				case PAJ_RIGHT:		printf("Right\r\n");
									system("echo \"next\" | nc 127.0.0.1 9294 -N");

				break;

				case PAJ_FORWARD:	printf("Forward\r\n");
									if (iRadioOn) {
									   system("echo \"stop\" | nc 127.0.0.1 9294 -N");
									   iRadioOn = FALSE;
									} 
									else {
                                       system("echo \"play\" | nc 127.0.0.1 9294 -N");
                                       iRadioOn = TRUE;
									}
				break;

				case PAJ_BACKWARD:			printf("Backward\r\n"); 		break;
				case PAJ_CLOCKWISE:			printf("Clockwise\r\n"); 		break;
				case PAJ_COUNT_CLOCKWISE:	printf("AntiClockwise\r\n"); 	break;
				case PAJ_WAVE:				printf("Wave\r\n"); 			break;
				default: break;

			}
			Gesture_Data=0;

		}

		delay(50);
	}

	return 0;
}


