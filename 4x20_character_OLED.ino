/*
Most character LCDs limit your character size to the standard 5x8 pixels.
Some character OLEDs, however, allow you to use built-in double-height fonts.
On a 2-line display, you can have 2-lines of normal sized text, or 1-line of double-height text.
On a 4-line display, you can have 4-lines of normal sized text, or 2 lines of double-height text, or even 1-line of double-height with 2-lines of normal text.
Here's an example of how a character OLED could be used in a vending machine application to change the font size depending on what text is desired.

This demo starts in 4-line normal sized font mode. The top line is supposed to draw your attention, but it doesn't because it's the same size as the rest of the text.
[HOLD DOWN KEYPAD BUTTON '0']
Now the top line is double-height and scrolling to really draw the customer's attention.
[10 seconds later, the screen will automatically change]
Again, the top line is double-height and scrolling, and now spaced apart from the lower text.
[Press button 'A', then button '4']
Again, the top line is double-height and scrolling at a different speed to signify action is happening.
[5 seconds later, the screen will automatically change]
Now, the top line is double-height, but the regular height 3rd line is scrolling.

The command to change from regular-height to double-height characters on this OLED is a single command to the Function Set register.
[Show screenshot of line 272]

Graphic LCDs and OLEDs give you the most flexibility on font and icons shapes and sizes, although they require a lot more program code,
  memory space, and MPU control of each individual pixel.

 */

#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
//#include <SPI.h>
//Pinout for Arduino MEGA to NHD-0420CW
//Pin1 = GND = GND
//Pin2 = VDD = +5V
//Pin3 = REGVDD = +5V
//Pin4 = D/C = 30
//Pin5 = R/W = GND
//Pin6 = E = 31
//Pin7-14 = DB0..DB7 = Digital pins 22..29 (PortA)
//Pin15 = /CS = 32
//Pin16 = /RES = 33
//Pin17-19 = BS0..2 = GND, GND, +5V
//Pin20 = GND = GND

int RS = 30;
int  E = 31;
int CS = 32;
int RES = 33;

int ASDA = 20;
int ASCL = 21;
int SCLK = 52;
int SDIN = 51;
int SDOUT = 50;

unsigned char tx_packet[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const char slave2w = 0x3C;  //3C or 78
unsigned char mode = 0; // 0 = 8-bit parallel 6800 mode; 1 = i2c mode; 2 = SPI mode;
int timer=0;
int timer2=0;
int fontSize=0;
int scrollSetting=0;
unsigned char text0[] =  {"                    "};
unsigned char text1[] =  {"  Ice Cold Drinks!  "};
unsigned char text2[] =  {"                    "};
unsigned char text3[] =  {"Insert coins, bills,or tap mobile device"};
unsigned char text4[] =  {"or tap mobile device"};

unsigned char text5[] =  {"Make your selection."};
unsigned char text6[] =  {"Selection A4 = $3.75   CREDIT: $ 0.25   "};
unsigned char text7[] =  {"      VENDING       "};
unsigned char text8[] =  {"                       CREDIT: $ 5.00   "};
unsigned char text9[] =  {"CREDIT: $ 1.25                          "};
  
unsigned char text10[] = {"     Thank you!     "};
unsigned char text11[] = {"Take your change...                     "};

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {//define the cymbols on the buttons of the keypads
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad
Keypad keypad1 = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

void command(unsigned char c)
{
  unsigned char i, temp;
  switch(mode)
  {
    case 0:   digitalWrite(CS, LOW);
              PORTA = c;
              digitalWrite(RS, LOW);
              digitalWrite(E, HIGH);
              delay(1);
              digitalWrite(E, LOW);
              digitalWrite(CS, HIGH);
              break;
   case 1:    tx_packet[0] = 0x00;
              tx_packet[1] = c;
              send_packet(2);
              break;
   case 2:    
              temp = 0xF8;
              for(i=0;i<8;i++)
              {
                digitalWrite(SCLK, LOW);
                if((temp&0x80)>>7==1)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                temp = temp << 1;
                digitalWrite(SCLK, HIGH);
              }
  
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                if((c&0x01)==1)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                c = c >> 1;
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                digitalWrite(SDIN, LOW);
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                if((c&0x10)==0x10)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                c = c >> 1;
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                digitalWrite(SDIN, LOW);
                digitalWrite(SCLK, HIGH);
              }
              break;
  }
  
}
void data(unsigned char d)
{
  unsigned char i, temp;
  switch(mode)
  {
    case 0:   digitalWrite(CS, LOW);
              PORTA = d;
              digitalWrite(RS, HIGH);
              digitalWrite(E, HIGH);
              delay(1);
              digitalWrite(E, LOW);
              digitalWrite(CS, HIGH);
              break;
   case 1:    tx_packet[0] = 0x40;
              tx_packet[1] = d;
              send_packet(2);
              break;
   case 2:    
              temp = 0xFA;
              for(i=0;i<8;i++)
              {
                digitalWrite(SCLK, LOW);
                if((temp&0x80)>>7==1)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                temp = temp << 1;
                digitalWrite(SCLK, HIGH);
              }
  
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                if((d&0x01)==1)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                d = d >> 1;
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                digitalWrite(SDIN, LOW);
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                if((d&0x10)==0x10)
                {
                  digitalWrite(SDIN, HIGH);
                }
                else
                {
                  digitalWrite(SDIN, LOW);
                }
                d = d >> 1;
                digitalWrite(SCLK, HIGH);
              }
              for(i=0;i<4;i++)
              {
                digitalWrite(SCLK, LOW);
                digitalWrite(SDIN, LOW);
                digitalWrite(SCLK, HIGH);
              }
              break;
  }
  
}

void send_packet(unsigned char x)
{
  unsigned char ix;
  Wire.beginTransmission(slave2w);
  for(ix=0;ix<x;ix++)
  {
    Wire.write(tx_packet[ix]);
  }
  Wire.endTransmission();
}


void output(unsigned char line1[], unsigned char line2[], unsigned char line3[], unsigned char line4[])
{
	int i;
	command(0x29);  //function set (extended command set and regular size font)
	fontSize=0;
  command(0x01);
  for(i=0;i<20;i++){data(line1[i]);}
  command(0xA0);
  for(i=0;i<20;i++){data(line2[i]);}
  command(0xC0);
  for(i=0;i<20;i++){data(line3[i]);}
  command(0xE0);
  for(i=0;i<20;i++){data(line4[i]);}
}

void output_double(unsigned char line1[], unsigned char line2[])
{
	int i;
	command(0x2C);  //function set (extended command set and Double Size font)
	fontSize=1;
  command(0x01);
  for(i=0;i<20;i++){data(line1[i]);}
  command(0xA0);
  for(i=0;i<40;i++){data(line2[i]);}
  }

void setup() 
{
  Serial.begin(9600);
  pinMode(ASCL, OUTPUT);      //set Arduino I2C lines as outputs
  pinMode(ASDA, OUTPUT);      //
  digitalWrite(ASCL, LOW);    //
  digitalWrite(ASDA, LOW);    //
  pinMode(SCLK, OUTPUT);
  pinMode(SDIN, OUTPUT);
  pinMode(SDOUT, INPUT);
  digitalWrite(SCLK, HIGH);
  digitalWrite(SDIN, HIGH);
  DDRA = 0xFF;
  PORTA = 0x00;
  DDRC = 0xFF;
  PORTC = 0x00;
  digitalWrite(RES, HIGH);
  delay(10);
  Wire.begin();
  delay(10);
  command(0x2A);  //function set (extended command set)
	command(0x71);  //function selection A, disable internal Vdd regualtor
	data(0x00);
	command(0x28);  //function set (fundamental command set)
	command(0x08);  //display off, cursor off, blink off
	command(0x1C);  //shift
	command(0x2A);  //function set (extended command set)
	command(0x79);  //OLED command set enabled
	command(0xD5);  //set display clock divide ratio/oscillator frequency
	command(0x70);  //set display clock divide ratio/oscillator frequency
	command(0x78);  //OLED command set disabled
	command(0x09);  //extended function set (4-lines)
  command(0x06);  //COM SEG direction
	command(0x72);  //function selection B, disable internal Vdd regualtor
	data(0x04);     //ROM CGRAM selection
	command(0x2A);  //function set (extended command set)
	command(0x79);  //OLED command set enabled
	command(0xDA);  //set SEG pins hardware configuration
	command(0x10);  //set SEG pins hardware configuration
	command(0xDC);  //function selection C
	command(0x00);  //function selection C
	command(0x81);  //set contrast control
	command(0x7F);  //set contrast control
	command(0xD9);  //set phase length
	command(0xF1);  //set phase length
	command(0xDB);  //set VCOMH deselect level
	command(0x40);  //set VCOMH deselect level
	command(0x78);  //OLED command set disabled
	command(0x28);  //function set (fundamental command set)
	command(0x01);  //clear display
	command(0x80);  //set DDRAM address to 0x00
	command(0x0C);  //display ON
  delay(100);
  
  keypad1.addEventListener(keypadEvent); //add an event listener for this keypad
  
  output(text1, text2, text3, text4);
}




void loop() 
{  
  char key = keypad1.getKey();   
  if (key){Serial.println(key);timer=0;}
  timer++;
  
  if(timer==30000){
    if(scrollSetting==0){;} //no scrolling
    if(scrollSetting==1){
      if(fontSize==0){command(0x29);}  //function set (extended command set and regular size font)
      else{
        command(0x2A);//Extension Register RE, double-height font
        command(0x1D);//UD2, UD1 = high, DH' = HIGH
        command(0x2D);//Extension Register IS set, double-height font
        command(0x2A);//Extension Register RE, double-height font
        command(0x11);//only scroll enable on line 1
        command(0x2C);//regular command set, double-height font - then 
        command(0x18);//Cursor or Display shift S/C=1, R/L=0
      }
    }
    if(scrollSetting==2){
      if(fontSize==0){command(0x29);
        command(0x2A);//Extension Register RE, double-height font
        command(0x1D);//UD2, UD1 = high, DH' = HIGH
        command(0x2D);//Extension Register IS set, double-height font
        command(0x2A);//Extension Register RE, double-height font
        command(0x12);//only scroll enable on line 2
        command(0x2C);//regular command set, double-height font - then 
        command(0x18);//Cursor or Display shift S/C=1, R/L=0
      }  //function set (extended command set and regular size font)
      else{
        command(0x2A);//Extension Register RE, double-height font
        command(0x1D);//UD2, UD1 = high, DH' = HIGH
        command(0x2D);//Extension Register IS set, double-height font
        command(0x2A);//Extension Register RE, double-height font
        command(0x12);//only scroll enable on line 2
        command(0x2C);//regular command set, double-height font - then 
        command(0x18);//Cursor or Display shift S/C=1, R/L=0
      }
    }
  timer=0;
  if(timer2>0){timer2++;}
  }
  if(timer2==50){
    output_double(text5, text8);
    scrollSetting = 1;//scroll line 1
    timer2=0;
  }
}


void keypadEvent(KeypadEvent key){//take care of some special events
  int i;
  switch (keypad1.getState()){
    case PRESSED:
      switch (key){
        case '4': 
          output_double(text7, text6);
          command(0x2A);//Extension Register RE, double-height font
          command(0x1D);//UD2, UD1 = high, DH' = HIGH
          command(0x2D);//Extension Register IS set, double-height font
          command(0x2A);//Extension Register RE, double-height font
          command(0x11);//only scroll enable on line 1
          command(0x2C);//regular command set, double-height font - then 
          for(i=0;i<50;i++){
            command(0x18);//Cursor or Display shift S/C=1, R/L=0
            delay(100);}
          output_double(text10, text11);
          scrollSetting=2;
        break;
      }
    break;
    case HOLD:
      switch (key){
        case '0': 
          output_double(text1, text3);
          timer2=1;//reset the "credit timer"
          scrollSetting = 1;//scroll line 1
        break;
        case '1': 
          output(text1, text2, text3, text4);
          timer2=1;//reset the "credit timer"
          scrollSetting = 0;//no scrolling
        break;
      }
    break;
  }
}
