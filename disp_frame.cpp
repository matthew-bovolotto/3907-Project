#include <energia.h>
#include <msp430.h>
#include <disp_frame.h>

#define DATA BIT3 //pin 2.3
#define CLK BIT4 //pin 2.4
#define LE BIT5 //pin 2.5


/*****************************************************************
BY: Matthew Bovolotto
Date: March 23rd 2016

Display Frame Function for LED Matrix
*****************************************************************/

//Pulses clock on P2.1 the number of times that are specified by integer i
frame_display::frame_display(){
  P2DIR = BIT3 + BIT4 + BIT5;
}

void frame_display::clock(short int i){
  for(i; i>0 ; i--){
    P2OUT |= CLK;
    delayMicroseconds(5);
    P2OUT &= ~CLK;
    delayMicroseconds(5);
  }
}

void frame_display::disp_frame(unsigned long frame[]){

unsigned long frame_shift = 0;
short int colour = 0;

for(int i=0; i<8; i++){ // for each column

  frame_shift = frame[i];
  colour = 0;

  for(int k=0; k<3; k++){ //For each colour
    P2OUT &=~ DATA; //clear row bits
    clock(8);
    P2OUT|= DATA; //give row bit
    clock(1);
    P2OUT &=~ DATA; //give 0 row bits
    clock(i);
    clock(2-k); //give leading 0's for red, green, blue

    for(int j=0; j<8; j++){ // For each row

        if((frame_shift & 1) == 1){    //Check the value is 1 and toggle pin
          P2OUT|= DATA;
        }else{
          P2OUT &=~ DATA;
        }

        clock(1); // clock data in

        if(j<7){
            P2OUT &=~ DATA; // clock 0's between colours if not last bit
            clock(2);
        }else{
          P2OUT &=~ DATA; // clock 0's on last bit depending on the colour
          if(colour == RED){clock(2);}
          if(colour == GREEN){clock(1);}
        }
        frame_shift = frame_shift >> 1; //shift the frame by 1

      }

    //LE Toggle
    P2OUT|= LE;
    delayMicroseconds(5);
    P2OUT &=~ LE;

    colour++;
  	}
  }
  clock(32);
  P2OUT|= LE;
  delayMicroseconds(5);
  P2OUT &=~ LE;
}


void frame_display::fade(int time){

unsigned long  shift_a[8] = {};
unsigned long  shift = 0;
unsigned long  arr[49] = {0,0x800000,0xC00000,0xE00000,0xF00000,0xF80000,0xFC0000,0xFE0000,0xFF0000,0x7F8000,0x3FC000,0x1FE000,0x0FF000,0x07F800,0x03FC00,0x01FE00,0x00FF00
                          ,0x007F80,0x003FC0,0x001FE0,0x000FF0,0x0007F8,0x0003FC,0x0001FE,0x0000FF,0x8000FF,0xC000FF,0xE000FF,0xF000FF,0xF800FF,0xFC00FF,0xFE00FF,0xFF00FF,
                        0x7F80FF,0x3FC0FF,0x1FE0FF,0x0FF0FF,0x07F8FF,0x03FCFF,0x01FEFF,0x00FFFF,0x80FFFF,0xC0FFFF,0xE0FFFF,0xF0FFFF,0xF8FFFF,0xFCFFFF,0xFEFFFF,0xFFFFFF};

   for(int i=0;i<49;i++){
      for(int j=0;j<8;j++){
       shift_a[j] = arr[i];
       }
       for(int k = 0; k<time;k++){
       disp_frame(shift_a);
       }
   }
}
