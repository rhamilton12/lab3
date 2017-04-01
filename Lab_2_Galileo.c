
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define STROBE	(40) // IO8
#define GPIO_A0	(48) // A0
#define GPIO_A1	(50) // A1
#define GPIO_A2	(52) // A2
#define GPIO_A3	(54) // A3
#define GPIO_DIRECTION_IN      (1)  
#define GPIO_DIRECTION_OUT     (0)
#define ERROR                  (-1)
#define HIGH	0x31
#define LOW	0x30

// User commands
#define MSG_RESET 0x0 // Reset the sensor to initial state.
#define MSG_PING 0x1 // Check if the sensor is working properly.
#define MSG_GET 0x2 // Obtain the most recent ADC result.

// Sensor Device Responses
#define MSG_ACK 0xE // Acknowledgement to the commands.
#define MSG_NOTHING 0xF // Reserved

// Define functions.
void enableGPIO(int gpio);// Enables a gpio for use.
void setGPIO(int gpio, int direction);// Set the GPIO to either read or write
void writeGPIO(int gpio, char val);// Write a value to a GPIO
char readGPIO(int gpio);	// Read a value from a GPIO
int storeData(int data0, int data1, int data2, int data3); // Convert the bit values of 4 gpios into an integer value.

//main
int main(void)
{
	// Declare variables
	int command; // Stores user command
	int readData;// Stores
	int results;// Used
 	int d0; // GPIO_A0
	int d1; // GPIO_A1
	int d2; // GPIO_A2
	int d3; // GPIO_A3
	int dStrobe;// GPIO STROBE

 	// Initilize GPIO pins
	enableGPIO(GPIO_A0);
	enableGPIO(GPIO_A1);
	enableGPIO(GPIO_A2);
	enableGPIO(GPIO_A3);
	enableGPIO(STROBE);

	setGPIO(GPIO_A0, GPIO_DIRECTION_OUT);
	setGPIO(GPIO_A1, GPIO_DIRECTION_OUT);
	setGPIO(GPIO_A2, GPIO_DIRECTION_OUT);
	setGPIO(GPIO_A3, GPIO_DIRECTION_OUT);
	setGPIO(STROBE, GPIO_DIRECTION_OUT);

        while(1)
        {
		// Prompt user for a command.
		printf("Press 0 -> reset\n");
		printf("      1 -> ping\n");
		printf("      2 -> data\n");
		scanf("%d", &command);
		
		// 
		if (command == MSG_RESET)
		{
			// Set strobe high to indicate to the pic that a command is incomming.
			writeGPIO(STROBE, HIGH);

			// Send ping command to the pic
			writeGPIO(GPIO_A0, LOW);
			writeGPIO(GPIO_A1, LOW);
			writeGPIO(GPIO_A2, LOW);
			writeGPIO(GPIO_A3, LOW);

			// Wait for 10 ms to ensure data is read and operated on by the pic.
			usleep(10000);

			// Set strobe low
			writeGPIO(STROBE, LOW);

			// Change direction of data lines to be able to read
			setGPIO(GPIO_A0, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A1, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A2, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A3, GPIO_DIRECTION_IN);
			usleep(10000);
			
			// Read data from pic
			d0 = readGPIO(GPIO_A0); // ASCII value of bit 0.
			d1 = readGPIO(GPIO_A1); // ASCII value of bit 1.
			d2 = readGPIO(GPIO_A2); // ASCII value of bit 2.
			d3 = readGPIO(GPIO_A3); // ASCII value of bit 3.
			
			// Store data from the pic into an integer value
			readData = storeData(d0, d1, d2, d3);
			
			// Change direction of data lines back to be able to write
			setGPIO(GPIO_A0, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A1, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A2, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A3, GPIO_DIRECTION_OUT);
						
			if (readData == MSG_ACK)
			{
				printf("Device reset.\n");
			}
			else
			{
				printf("Device did not reset.\n");
			}
		}

		else if (command == MSG_PING)
		{
			// Set strobe high to indicate to the pic that a command is incomming.
			writeGPIO(STROBE, HIGH);

			// Send ping command to the pic
			writeGPIO(GPIO_A0, HIGH);
			writeGPIO(GPIO_A1, LOW);
			writeGPIO(GPIO_A2, LOW);
			writeGPIO(GPIO_A3, LOW);

			// Wait for 10 ms to ensure data is read and operated on by the pic.
			usleep(10000);

			// Set strobe low
			writeGPIO(STROBE, LOW);

			// Change direction of data lines to be able to read
			setGPIO(GPIO_A0, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A1, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A2, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A3, GPIO_DIRECTION_IN);
			usleep(10000);
			
			// Read data from pic
			d0 = readGPIO(GPIO_A0); // ASCII value of bit 0.
			d1 = readGPIO(GPIO_A1); // ASCII value of bit 1.
			d2 = readGPIO(GPIO_A2); // ASCII value of bit 2.
			d3 = readGPIO(GPIO_A3); // ASCII value of bit 3.
			
			// Store data from the pic into an integer value
			readData = storeData(d0, d1, d2, d3);
			
			// Change direction of data lines back to be able to write
			setGPIO(GPIO_A0, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A1, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A2, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A3, GPIO_DIRECTION_OUT);
						
			if (readData == MSG_ACK)
			{
				printf("Device ready.\n");
			}
			else
			{
				printf("Device no responding.\n");
			}
		}
		else if (command == MSG_GET)
		{
			// Set strobe high to indicate to the pic that a command is incomming.
			writeGPIO(STROBE, HIGH);

			// Send ping command to the pic
			writeGPIO(GPIO_A0, LOW);
			writeGPIO(GPIO_A1, HIGH);
			writeGPIO(GPIO_A2, LOW);
			writeGPIO(GPIO_A3, LOW);

			// Wait for 10 ms to ensure data is read and operated on by the pic.
			usleep(10000);

			// Set strobe low
			writeGPIO(STROBE, LOW);

			// Change direction of data lines to be able to read
			setGPIO(GPIO_A0, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A1, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A2, GPIO_DIRECTION_IN);
			setGPIO(GPIO_A3, GPIO_DIRECTION_IN);
			usleep(10000);
			
			results = 0;
			readData = 0;
			// First set of data
			d0 = readGPIO(GPIO_A0); // ASCII value of bit 0.
			d1 = readGPIO(GPIO_A1); // ASCII value of bit 1.
			d2 = readGPIO(GPIO_A2); // ASCII value of bit 2.
			d3 = readGPIO(GPIO_A3); // ASCII value of bit 3.
			
			// Store data from the pic into an integer value
			results = storeData(d0, d1, d2, d3);
			writeGPIO(STROBE, HIGH);
			usleep(10000);
			writeGPIO(STROBE, LOW);
			usleep(10000);
			
			// Second set of data
			d0 = readGPIO(GPIO_A0); // ASCII value of bit 0.
			d1 = readGPIO(GPIO_A1); // ASCII value of bit 1.
			d2 = readGPIO(GPIO_A2); // ASCII value of bit 2.
			d3 = readGPIO(GPIO_A3); // ASCII value of bit 3.
			
			// Store data from the pic into an integer value
			readData = storeData(d0, d1, d2, d3);
			readData <<= 4;
			results = results | readData;
			writeGPIO(STROBE, HIGH);
			usleep(10000);
			writeGPIO(STROBE, LOW);
			usleep(10000);
			
			// Third set of data
			d0 = readGPIO(GPIO_A0); // ASCII value of bit 0.
			d1 = readGPIO(GPIO_A1); // ASCII value of bit 1.
			d2 = readGPIO(GPIO_A2); // ASCII value of bit 2.
			d3 = readGPIO(GPIO_A3); // ASCII value of bit 3.
			
			// Store data from the pic into an integer value
			readData = storeData(d0, d1, d2, d3);
			readData <<= 8;
			results = results | readData;
			
			// Change direction of data lines back to be able to write
			setGPIO(GPIO_A0, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A1, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A2, GPIO_DIRECTION_OUT);
			setGPIO(GPIO_A3, GPIO_DIRECTION_OUT);

			printf("ADC reults: %d\n", results);
			
		}
		else
		{
			printf("Command not valid.\n\n");
		}
        }
}

// Enables a gpio for use.
void enableGPIO(int gpio)
{
	char buffer[256];
	int fileHandle;

	//Export GPIO
	fileHandle = open("/sys/class/gpio/export", O_WRONLY);
	if(ERROR == fileHandle)
	{
		puts("Error: Unable to opening /sys/class/gpio/export");
		exit(1);
	}
	sprintf(buffer, "%d", gpio);
	write(fileHandle, buffer, strlen(buffer));
	close(fileHandle);
}

// Set the GPIO to either read or write
void setGPIO(int gpio, int direction)
{
	char buffer[256];
        int fileHandle;
        int fileMode;
	
   	//Direction GPIO
        sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               exit(1);
        }
        if (direction == GPIO_DIRECTION_OUT)
        {
               // Set out direction
               write(fileHandle, "out", 3);
               fileMode = O_WRONLY;
        }
        else
        {
               // Set in direction
               write(fileHandle, "in", 2);
               fileMode = O_RDONLY;
        }
        close(fileHandle);
}


// Write a value to a GPIO
void writeGPIO(int gpio, char val)
{
	char buffer[256];
        int fileHandle;
	
	//Setup GPIO to write
        sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
		puts("Unable to open file:");
               	puts(buffer);
               	exit(1);
        }
	write(fileHandle, &val, 1);
        close(fileHandle);  //This file handle will be used in write and close operations.	
}

// Read a value to a GPIO
char readGPIO(int gpio)
{
	char buffer[256];
	char data;
        int fileHandle;
	
	//Setup GPIO to read
        sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
        fileHandle = open(buffer, O_RDONLY);
        if(ERROR == fileHandle)
        {
		puts("Unable to open file:");
               	puts(buffer);
               	exit(1);
        }
	read(fileHandle, &data, 1);// Read 1 byte of data from fileHandle and place it in data
        return(data);  //This file handle will be used in read and close operations.	
}

// Convert the bit values of 4 gpios into an integer value.
int storeData(int data0, int data1, int data2, int data3)
{
	int data;
	
	// Convert data from its ASCII value to 0 or 1
	data0 &= 0x01;
	data1 &= 0x01;
	data2 &= 0x01;
	data3 &= 0x01;
	
	// Store bit values data0 - data3 into data
	data = 0;
	data = data0;
	data |= (data1 << 1);
	data |= (data2 << 2);
	data |= (data3 << 3);
	
	return data;
}


