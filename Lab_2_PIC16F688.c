
#include <htc.h> 
#include <pic16f18857.h>
#include <xc.h>

// Must define frequency if we want delay function
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000 // Set crystal frequency to 8 MHz.
#endif 

// CONFIG

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

// User messages
#define MSG_RESET 0x00 // Reset the sensor to initial state.
#define MSG_PING 0x01 // Check if the sensor is working properly.
#define MSG_GET 0x02 // Obtain the most recent ADC result.

// Define functions.
void set_ADC(); // Used to initialize the ADC
void set_receive(); // Used to ensure that the PIC16F18857 is initialized correctly
                    // so that it can 'talk' to the Galileo.
void receive_msg(); // Used to interpert a message from the Galileo and to respond accordingly.

// Main program.
int main(void)
{
    // Wait for the power supply to settle.
    __delay_ms(10);
    // Initialize the ADC.
    set_ADC();
    
    while(1){
        receive_msg();
        set_ADC();
        //ADGO = 1;		// Set the ADC so that it is actively reading.
        
		// When the ADC is actively reading enter the loop.
        while(ADGO != 0){
            if (ADRESH < 0x7F)	// If result is low
                RC0 = 1;        // Turn the LED on
            else                // If result is high
                RC0 = 0;        // Turn the LED off
        }
    }
}

// Used to initialize the ADC
void set_ADC(){
    // The following sets all pins to digital I/O.
    PORTA = 0x00;   // Pins AN0 - AN7 are set to 0.
    ANSELC = 0x00;  // CxIN pins are configured as digital I/O
    TRISC = 0;              // This tells the pin to be an output
    RC0 = 0;                // This sets the initial state to 0V.
    	// Setting pin 11 of the PIC16F18857 to be an analog input.
    TRISA = 0b00000001;     // Set AN0 to input
    ANSELA = 0b00000001;     // Set AN0 to analog
    ADCON1 = 0b01010000;    // Set ADC conversion clock to FOSC/16 range.
	// Setup the ADCON0 register
   ADCON0=0x00;
   ADON=1;  //switch on the adc module
   ADGO=1;//Start conversion
   ADREF = 0x00;	// Set reference voltage of the ADC to Vdd.	// ADC conversion result format left justified.
   while(ADGO); //wait for the conversion to finish
   ADON=0;  //switch off adc
   //PIR1bits.ADIF = 0;		// ADC interrupt flag is not enabled.
}

// Used to ensure that the PIC16F18857 is initialized correctly so that it can 'talk' to the Galileo.
void set_receive(){
    // Ensures that STROBE and D0 - D3 are treated as inputs.
    TRISC |= 0x3E;  // RC1 - RC5 are set to 1.
}

// Used to interpret a message from the Galileo and to respond accordingly.
void receive_msg(){
    static int message; // Used to store the message from the Galileo.
    static int strobe_low; // Used to check if the PIC needs to respond to a message from the Galileo.
    static int ready; // Check to see if the Galileo is ready for the next set of data sent by the PIC.
    int temp = 0; // Used to temporarily store values before storing them in either the message from the Galileo or the results from the ADC.
    int results; // Used to store the results of the ADC
    int i; // For indexing a loop.
    int n; // Used to breakout of a while loop that is waiting for the Galileo to be ready for the next set of data.
    
    set_receive();
    
    if ((PORTCbits.RC5 == 1) &&(!strobe_low)){
        // Once the strobe signal is high
        __delay_ms(10);
        
        // Storing the message sent from the Galileo.
        message = 0; // Initialize message.
        message = PORTCbits.RC1;
        temp = PORTCbits.RC2;
        message |= (temp << 1);
        temp = PORTCbits.RC3;
        message |= (temp << 2);
        temp = PORTCbits.RC4;
        message |= (temp << 3);
        strobe_low = 1;
    }
    else if((PORTCbits.RC5 == 0)&&(strobe_low))
    {
        strobe_low = 0;
        if (message == MSG_RESET){
            //Reset the ADC
            ADCON0=0x00;
            ADRESH = 0x00;
            ADRESL = 0x00;
            //Set D0 - D3 as outputs.
            TRISC &= 0x21;
            //Write "Acknowledged" (0xE) command to Galileo.
            PORTCbits.RC1 = 0;
            PORTCbits.RC2 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 1;
            __delay_ms(10);   
        }else if (message == MSG_PING){
            //Set D0 - D3 as outputs.
            TRISC &= 0x21;
            //Write "Acknowledged" (0xE) command to Galileo.
            PORTCbits.RC1 = 0;
            PORTCbits.RC2 = 1;
            PORTCbits.RC3 = 1;
            PORTCbits.RC4 = 1;
            __delay_ms(10);
        }else if (message == MSG_GET){
            //Capture results of the ADC.
            results = 0x0000;
            results = ADRESH;
            results <<= 2;
            temp = ADRESL & 0xC0;
            temp = temp >> 6;
            results |= temp;
            
            //Set D0 - D3 as outputs.
            TRISC &= 0x21;
            
            //Send ADC data to the Galileo.
            for(i = 0; i < 3; i++)
            {
                PORTCbits.RC1 = results & 0x001;
                results >>= 1;
                PORTCbits.RC2 = results & 0x001;
                results >>= 1;
                PORTCbits.RC3 = results & 0x001;
                results >>= 1;
                PORTCbits.RC4 = results & 0x001;
                results >>= 1;
                ready = PORTCbits.RC5;
                n = 0;
                while((PORTCbits.RC5 == ready)){
                    //Wait for the strobe to go high, which indicates to the PIC that the Galileo is ready for the next set of data.
                    n++;
                    __delay_ms(1);
                    if (n == 10){
                        break;
                    }
                }
                __delay_ms(10);
            }
        }
    }
}