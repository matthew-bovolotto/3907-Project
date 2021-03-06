#include <msp430.h>
#include <IR_receiver.h>
#include <energia.h>

// the setup routine runs once when you press reset:
IR_receiver::IR_receiver() {
  P2DIR |= BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
  P1DIR |= BIT6;
  P1DIR &= ~BIT7;
}


// the loop routine runs over and over again forever:
void IR_receiver::IR_get(unsigned int spiValue[], int &threshold, int &threshcount) {
  //for loop variables
  int i;
  int j;

  //set to the max possible value of analogRead()
  // threshold = 400;
  //value read in from the IR LEDs
  int sensorValue;
  //2D array to store IR LED values
  int frame[sizex][sizey];
  //2D array to store IR LED values as 'high' or 'low'
  int frame2[sizex][sizey];
  int threshold2;
  //these store the information in frame2 to be sent to the RGB matrix

  //setThreshold count and boolean values



  //sending a 1 to the shift register
  P1OUT |= BIT6;
  //shift register loop
  for (int i = 0; i < sizex; i++)
  {
    //delayMicroseconds(5);
    //Shift register stores a value
    P2OUT |= BIT7;
    //delayMicroseconds(5);
    //shift register shifts in the value stored from BIT7
    //it also shifts all other values to the left
    P2OUT |= BIT6;
    //delayMicroseconds(5);
    //sending the shift register a 0 for next time
    P1OUT &= ~BIT6;
    //setting the 'clock' signals low for next time
    P2OUT &= ~BIT6 + ~BIT7;
    //Serial.print(threshcount);
    //multiplexer loop
    for (int j = 0; j < sizey;j++)
    {
      //Sets A, B, and C to low
      P2OUT &= ~BIT3 + ~BIT4 + ~BIT5;
      if ((j % 2) == 1)
      {
        //sets A to high if the current multiplexer input
        //is an odd number
        P2OUT |= BIT3;
      }
      if ((j % 4) > 1)
      {
        //sets B to high if the current input is 2,3,6 or 7
        P2OUT |= BIT4;
      }
      if (j > 3)
      {
        //Sets C to high if the current input is higher than 3
        P2OUT |= BIT5;
      }

      //delayMicroseconds(5)

      frame[i][j] = analogRead(A7);
    //  for(j = 0; j <sizey; j++)
    //  {
        if ( j == 0)
          threshold2 = 580;
        else if (j == 1)
          threshold2 = 380;
        else if (j == 2)
        threshold2 = 210;
        else if (j == 3)
        threshold2 = 130;
        else if (j == 4)
        threshold2 = 75;
        else if (j == 5)
        threshold2 = 60;
        else
          threshold2 = 35;

       if (frame[i][j] < (threshold2) && frame[i][j] > 4)
       {
           frame2[i][j] = 1;
       }
       else
       {
          //low value detected
          frame2[i][j] = 0;
       }
    //}
  }
  //sets threshhold boolean to low
  //for (i = 0; i <sizex; i++)
  //{



     //shifts through storing values in the long int

  //}
  }
  for (i = 0; i <4; i++)
  {
    for (j = 0;j < 4 ; j++)
      {
        if(frame2[2*i][2*j] == 1 || frame2[2*i + 1][2*j + 1] == 1)
        {
        frame2[2*i][2*j] = 1;
        frame2[2*i + 1][2*j] = 1;
        frame2[2*i][2*j + 1] = 1;
        frame2[2*i + 1][2*j + 1] = 1;
      //  spiValue |= 1;

        //if (j!=3 && i!= 3)
      //  spiValue = spiValue[0] << 1;
        }
      }
    }
      int spicount = 0;
    for(j = 7;j > -1; j--)
    {
      for(i = 0; i < 8; i++)
      {
        if (spicount < 16){
          //if (spicount != 0)
          spiValue[1] = spiValue[1] << 1;
          spiValue[1] += frame2[i][j];

        }else if(spicount < 32){
          //if (spicount != 16)
          spiValue[0] = spiValue[0] << 1;
          spiValue[0] += frame2[i][j];

        }else if(spicount < 48){
          //if (spicount != 32)
          spiValue[3] = spiValue[3] << 1;
          spiValue[3] += frame2[i][j];

        }else{
          //if (spicount != 48)
          spiValue[2] = spiValue[2] << 1;
          spiValue[2] += frame2[i][j];

        }
        spicount++;
      }
    }

 //if a high value was detected resets the count

  //no high values were detected, increments the count
    threshcount++;

   //if twenty cycles go by with no high values, calls setThreshold
  if (threshcount == numcycle)
  {
    //setThreshold(frame, threshold, threshcount);
    //threshcount = 0;
  }

  //Prints the output to serial
  for (i = 0; i <sizex; i++)
  {
    for(j = 0; j <sizey; j++)
    {
      //Serial.print(frame[i][j]);
      //Serial.print("\t");
      #ifdef SERIAL
      Serial.print(frame[i][j]);
      Serial.print("\t");
      #endif

    }
    #ifdef SERIAL
    Serial.println(" ");
    #endif
  }
  #ifdef SERIAL
  Serial.println(" ");
  #endif
//Prints the output to serial in the form of 'high'/'low'
}


//setsa new threshold value for each led based on the current value
void IR_receiver::setThreshold(int frame[][sizey], int &threshold, int &threshcount)
{

  int i;
  int j;
  int value = 0;
  int count = 0;
  for (i = 0; i <sizex; i++)
  {
    for(j = 0; j <sizey; j++)
    {
      if (frame[i][j] > (threshold))
      {
      value += frame[i][j];
      count++;
      }
    }
  }
  //threshold = value/count - threshval;
  threshold = 20;
}
