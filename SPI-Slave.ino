#include <msp430.h>
#include <spi_library.h>
#include <disp_frame.h>

//#define SERIAL

//#define SPI
//#define BLUETOOTH

void setup()
{
  #ifdef SERIAL
    
  #endif
  Serial.begin(9600); // begin serial communication
  Serial.println("-Board Start Successful- S");
  pinMode(1, OUTPUT);
  pinMode(2, INPUT);
  pinMode(9, INPUT_PULLUP);//button read in
}

unsigned int checkvalue[4] = {0,0,0,0}; //define variable for SPI to write into
unsigned long frame[8];
unsigned long frame2[8] = {0x00FF,0,0,0,0x00FF,0,0,0};
unsigned long frame3[8] = {0xFF00,0,0,0,0xFF00,0,0,0};
unsigned long frame4[8] = {0xFF0000,0,0,0,0xFF0000,0,0,0};
unsigned long inputarray[8];
char x;

spi_Slave spi;
frame_display dispfram;

void loop(){
    
  #ifdef SPI
  spi.spiReceive_u16x4(checkvalue);
    Serial.println("hello world");
    
      #ifdef SERIAL
      Serial.print("Checkvalue = ");
      Serial.println(checkvalue[0]);
      Serial.println(checkvalue[1]);
      Serial.println(checkvalue[2]);
      Serial.println(checkvalue[3]);
      #endif
   
      for(int i=0; i<8; i+=2){
       frame[i] = checkvalue[i/2] & 0xFF ;
       frame[i+1] = (checkvalue[i/2] & 0xFF00) >> 8; 
      }
      
      #ifdef SERIAL
      Serial.print("Checkvalue = ");
      for(int i=0; i<8; i++){
		Serial.println(frame[i], HEX);
      }
      #endif
    for(int i = 0; i < 50; i++){
		dispfram.disp_frame(frame);
    }
   #endif
    
    #ifdef BLUETOOTH
    while(Serial.available() == 0){dispfram.disp_frame(inputarray);};
    for(int i=0; i < 8; i++){
    inputarray[i] = 0;
    }
    if(Serial.available() > 0){
		x = Serial.read();
		while(Serial.available() == 0){};
		x = Serial.read();
		while(Serial.available() == 0){};
		x = Serial.read();
		while(Serial.available() == 0){};
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 24; j++){
				while(Serial.available() == 0){};
				x = Serial.read();
				if(x == 49){inputarray[i] |= 1;}
				if(j < 23){inputarray[i] = inputarray[i] << 1;}
			}
		}
		Serial.println();
   
		for(int i=0; i < 8; i++){
			Serial.println(inputarray[i], BIN);
		}
    
		Serial.println();
		}
    }
	#endif
}



