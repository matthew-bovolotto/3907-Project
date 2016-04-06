//Definitions

//Includes
#include <Energia.h>
#include <msp430.h>
#include <spi_library.h>

/*************************************************
Matthew Bovolotto -- February 29th 2016

Pseudo SPI library for the MSP430(G2253) MCU

Contains important functionality for pseudo spi including:
- SPI setup for Slave/Master
- SPI 16 bit recieve and send

To be implemented:

- SPI 4 bit header mask to deal with different length messages
as well as different types

- Header will define type of integer being passed

0000 - Null value, no more values
xx01 - 8 bit integer
xx10 - 16 bit integer
xx11 - 32 bit integer

top 2 values are empty at the moment

after header the microcontroller will begin receiving data of the specified type until null value is received

*************************************************/

spi_Master::spi_Master(){
  spi_Master_Setup();
}

void spi_Master::spi_Master_Setup(){
  /* Begin Setup of Pseudo SPI
   P1.3 - CLK
   P1.4 - MOSI
   P1.5 - MISO
   P2.0 - SS
   */
  P1DIR = BIT3 + BIT4; // enable pins 1.3, 1.4 CLK, MOSI
  P1DIR &= ~BIT5; // enable pin 1.5 MISO
  P2DIR = BIT0; // enable pin 2.0 SS

  #ifdef SERIAL
    Serial.println("SPI Setup successful");
  #endif
}

bool spi_Master::spiSend(unsigned int spi_data_out, int length){

  /* Function writes a 16 bit character to the slave driver

  1. Set SS high to signal beginning of write
  2. Wait until acknowledged by the slave
  3. Begin passing 16 bits of data
  */

  unsigned int watchdog = 0; //set up watchdog

  P2OUT |= BIT0; // force SS high to notify slave to begin receiving
  while(((P1IN & BIT5) >> 5) == 0){ // Wait for Slave acknowledgement
    watchdog+=1;
    if(watchdog > 160){ // check for watchdog timeout
      P2OUT &= ~BIT0;
      #ifdef SERIAL
        Serial.println("error: watchdog timeout");
      #endif
      return 1;
    }
    delayMicroseconds(100);
  }

  unsigned long int output = 0; // setup masked output variable

  for(int i = 0; i < length; i++){
    P1OUT &= ~BIT3; // set clock high
    output = spi_data_out & 1;
    if(output == 1){
      P1OUT |= BIT4; // set MOSI to 1
    }else{
      P1OUT &= ~BIT4; // set MOSI to 0
    }
    delayMicroseconds(10);
    P1OUT |= BIT3;  // set clock low
    delayMicroseconds(10);
    spi_data_out = spi_data_out >> 1; // shift data out by 1
  }
  P2OUT &= ~BIT0; // close SS bit
  P1OUT &= ~BIT4;
  delayMicroseconds(5);
  return 0;
}

// SPI Send methods to help make selection of data type sending easy

bool spi_Master::spiSend_Header(unsigned int spi_data_out){bool status = spiSend(spi_data_out,4); return status;}

bool spi_Master::spiSend_u8(unsigned long int spi_data_out){bool status = spiSend(spi_data_out,8); return status;}

bool spi_Master::spiSend_u16(unsigned long int spi_data_out){bool status = spiSend(spi_data_out,17); return status;}

bool spi_Master::spiSend_u32(unsigned long int spi_data_out){bool status = spiSend(spi_data_out,32); return status;}

/******************************************************
*               SPI Slave Declarations                *

*                                                     *

*                                                     *
******************************************************/

spi_Slave::spi_Slave(){
  spi_Slave_Setup();
}

void spi_Slave::spi_Slave_Setup(){

  /* Begin Setup of Pseudo SPI
   P1.3 - CLK
   P1.4 - MOSI
   P1.5 - MISO
   P2.0 - SS
   */

   P1DIR &= ~BIT3 + ~BIT4; // setup pins 1.3, 1.4 as CLK MOSI
   P1DIR |= BIT5; // setup pin 1.5 as MISO
   P2DIR &= ~BIT0; // setup pin 2.0 as SS
}

unsigned long int spi_Slave::spiReceive(int length){

  /* Function reads a 16 bit character to the slave driver
  ***** GENERALLY USED WITH AN INTERRUPT ENABLED ON THE SLAVE *****

  1. Send acknowledge signal to master device
  2. Wait for high clock signal and read data when valid edge
  3. Wait for next clock edge to read until 16 bits received
  4. Return 16 bit unsigned integer value

  */

  unsigned long int spi_data_in = 0; // setup data input variable
  int watchdog = 0; // setup watchdog
  P1OUT |= BIT5; // give acknowledge to the master

  for(int i = 0; i < length; i++){

    watchdog=0; // reset watchdog
    while(((P1IN & BIT3) >> 3) == 0){ // check for rising edge of clock
      watchdog+=1;
      if(watchdog > 1000){  //check watchdog
        P1OUT &= ~BIT5;  //set acknowledge know in timeout case
        #ifdef SERIAL
          if(watchdog % 100){
          Serial.println("error: watchdog timeout 1");
          }
        #endif
        return 0;
      }
    }

    spi_data_in += (((P1IN & BIT4) >> 4) << i-1); // add value read into the data in variable

    watchdog=0; // reset watchdog
    while(((P1IN & BIT3) >> 3) == 1){ // check for falling edge of clock
      watchdog+=1;
      if(watchdog > 1000){   //check watchdog
        P1OUT &= ~BIT5; //set acknowledge low in timeout case
        #ifdef SERIAL
          if(watchdog % 100){
          Serial.println("error: watchdog timeout 2");
          }
        #endif
        return 0;
      }
    }
  }
  P1OUT &= ~BIT5; // set acknowledge low
  return spi_data_in; // return final value read
}

// SPI recieve methods, to make it easy for the user predefined length recieve methods are defined
// Once header is better implemented the header method will be able to take care of running the necessary
// method required.

unsigned long int spi_Slave::spiReceive_header(){
  unsigned long int spi_data_in = spiReceive(4);
  switch(spi_data_in){
    case 0b0000:
      spi_data_in = 0;
			break;
    case 1:
      spi_data_in = spiReceive_u8();
      break;
    case 2:
      spi_data_in = spiReceive_u16();
      break;
    case 3:
      spi_data_in = spiReceive_u32();
      break;
		default:
			spi_data_in = 0;
			break;
	}
  return spi_data_in;
}

unsigned long int spi_Slave::spiReceive_u8(){unsigned long int spi_data_in = spiReceive(8); return spi_data_in;}
unsigned long int spi_Slave::spiReceive_u16(){unsigned long int spi_data_in = spiReceive(17); return spi_data_in;}
unsigned long int spi_Slave::spiReceive_u32(){unsigned long int spi_data_in = spiReceive(32); return spi_data_in;}

void spi_Slave::spiReceive_u16x4(unsigned int checkvalue[]){
  int watchdog;
  for(int i = 0; i<4;){
    watchdog++;
    if((P2IN & BIT0) == 1){ // check if SS bit has been set high
       checkvalue[i] = spiReceive_header();
       i++;
       delayMicroseconds(20);
    }else if(watchdog > 1000){
      return;
    }
  }
}
