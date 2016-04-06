#include <msp430.h>
#include <spi_library.h>
#include <IR_receiver.h>
//#define SERIAL

//#define IR
#define SPI

// the setup routine runs once when you press reset:

void setup() {
#ifdef SERIAL
  Serial.begin(9600);
  Serial.println("-Board Start Successful- M");
#endif
}

int return_value = 0;
unsigned int spi_data_out[4] = {0xAA55,0xAA55,0xAA55,0xAA55};
unsigned int spi_data_out2[4] = {0x55AA,0x55AA,0x55AA,0x55AA};
unsigned int IR_frame[4] = {0,0,0,0};
int threshold = 200;
int threshcount = 0;

void loop(){
  
  IR_receiver IRreceive;
  
  IRreceive.IR_get(IR_frame, threshold, threshcount);
  //if(Serial.available()){
    //spi_data_out = Serial.read() - 48;
  
  for(int i = 0; i<4; i++){
    
    spi_Master spi; //create object spi -- setups all required pins for spi

    do{
      return_value = spi.spiSend_Header(2); // sends header with value 2 -- 16 bit unsigned integer
 
    }
    while(return_value > 0); // check to make sure header didnt timeout


    do{
	#ifdef SPI
      return_value = spi.spiSend_u16(spi_data_out[i]); // sends 16 bit val
	#endif 
	#ifdef IR
	  return_value = spi.spiSend_u16(IR_frame[i]); // sends 16 bit val
	#endif

    }
    while(return_value > 0); // check to make sure data didnt timeout
    #ifdef SERIAL
    Serial.println(IR_frame[i], BIN);
    #endif
  } 
    
  for(int i = 0; i<4; i++){
    
    spi_Master spi; //create object spi -- setups all required pins for spi

    do{
      return_value = spi.spiSend_Header(2); // sends header with value 2 -- 16 bit unsigned integer
 
    }
    while(return_value > 0); // check to make sure header didnt timeout


    do{
	#ifdef SPI
      return_value = spi.spiSend_u16(spi_data_out2[i]); // sends 16 bit value
	#endif
	#ifdef IR
	  return_value = spi.spiSend_u16(IR_frame[i]); // sends 16 bit value
	#endif

    }
    while(return_value > 0); // check to
  }
}

