#include <Wire.h>

#define PWM_PIN 3
#define LCD_ADDR 0x3D  // I2C address of LPH7470-1 LCD display
#define LCD_WIDTH 0x60 // display width in px

#define TRG_PIN 4 // sonar trigger pin
#define ECH_PIN 5 // sonar echo pin

int temp = 20; //Celsius
double speedofsound = 33150 + (60 * temp); // in cm

byte line = 0x60; //0x60 - 0x64, each byte represents 8 lines, exept 0x64 which represents the last 3px
byte column = 0x00; //0x00 - 0x60, each byte represents 1 column, after 0x61 the display automatically moves to the next line
//byte bitmap = 0xFF; //0x00 - 0xFF, each byte represents a bitmap for 8 vertical pixels addressed by line and column


void setup()
{
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)

  // sonar module
  pinMode(TRG_PIN, OUTPUT);
  pinMode(ECH_PIN, INPUT);

  // lcd screen
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, 127); //output 3 volt PWM signal on pin 3 for the voltage inverter (negative voltage required for LCD contrast, without this, writing to display will not be possible)
  lcdclear();
  
  setCursorPos(0, 0);
  lcdprint("Dist.:");
}

void loop()
{
  double distance = getDistance();
  
  String number = String(distance);     // convert number to string
  String text = rightboundtext(9, String(number + "cm"));
  setCursorPos(LCD_WIDTH - 6 * text.length(), 0); // right bound text
  lcdprint(text);
  
  delay(400);
}

double getDistance()
{
  digitalWrite(TRG_PIN, LOW);
  delayMicroseconds(3);
  
  noInterrupts();
  digitalWrite(TRG_PIN, HIGH); //Trigger Impuls 10 us
  delayMicroseconds(10);
  digitalWrite(TRG_PIN, LOW);
  double etime = pulseIn(ECH_PIN, HIGH) / 2; // Echo time of one way in us
  interrupts();
  
  double dist = speedofsound * (etime / 1000000); //cm/s * s = cm
  return(dist);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LPH7470-1 LCD Grafic Matrix
//
// resolution 97x35px
// 35 % 8 = 4 remaining 3px for byte 0x64
//
//        0x00 0x01 0x02 0x03
//  0x60  O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...
//  0x61  O    O    O    O    ...
//  "     O    O    O    O    ...
//  "     O    O    O    O    ...

byte font[][6] = {
                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0x20  
                   {0x00, 0x00, 0x5f, 0x00, 0x00, 0x00}, // 0x21 !
                   {0x00, 0x07, 0x00, 0x07, 0x00, 0x00}, // 0x22 "
                   {0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00}, // 0x23 #
                   {0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00}, // 0x24 $
                   {0x23, 0x13, 0x08, 0x64, 0x62, 0x00}, // 0x25 %
                   {0x36, 0x49, 0x55, 0x22, 0x50, 0x00}, // 0x26 &
                   {0x00, 0x05, 0x03, 0x00, 0x00, 0x00}, // 0x27 '
                   {0x00, 0x1c, 0x22, 0x41, 0x00, 0x00}, // 0x28 (
                   {0x00, 0x41, 0x22, 0x1c, 0x00, 0x00}, // 0x29 )
                   {0x14, 0x08, 0x3e, 0x08, 0x14, 0x00}, // 0x2A *
                   {B00001000,
                    B00001000,
                    B00111110,
                    B00001000,
                    B00001000,
                    B00000000}, // 0x2B +
                   {B00000000,
                    B11100000,
                    B00000000,
                    B00000000,
                    B00000000,
                    B00000000}, // 0x2C ,
                   {B00001000,
                    B00001000,
                    B00001000,
                    B00001000,
                    B00001000,
                    B00000000}, // 0x2D -
                   {B00000000,
                    B01100000,
                    B00000000,
                    B00000000,
                    B00000000,
                    B00000000}, // 0x2E .
                   {0x20, 0x10, 0x08, 0x04, 0x02, 0x00}, // 0x2F /
                   {B00111110,
                    B01010001,
                    B01001001,
                    B01000101,
                    B00111110,
                    B00000000}, // 0x30 0
                   {B01000000,
                    B01000010,
                    B01111111,
                    B01000000,
                    B01000000,
                    B00000000}, // 0x31 1
                   {B01100010,
                    B01010001,
                    B01001001,
                    B01001001,
                    B01000110,
                    B00000000}, // 0x32 2
                   {B00100010,
                    B01000001,
                    B01001001,
                    B01001001,
                    B00110110,
                    B00000000}, // 0x33 3
                   {B00011000,
                    B00010100,
                    B00010010,
                    B00010001,
                    B01111111,
                    B00000000}, // 0x34 4
                   {B00100111,
                    B01000101,
                    B01000101,
                    B01000101,
                    B00111001,
                    B00000000}, // 0x35 5
                   {B00111100,
                    B01001010,
                    B01001001,
                    B01001001,
                    B00110000,
                    B00000000}, // 0x36 6
                   {B00000011,
                    B00000001,
                    B01110001,
                    B00001001,
                    B00000111,
                    B00000000}, // 0x37 7
                   {B00110110,
                    B01001001,
                    B01001001,
                    B01001001,
                    B00110110,
                    B00000000}, // 0x38 8
                   {B00000110,
                    B01001001,
                    B01001001,
                    B01001001,
                    B00111110,
                    B00000000}, // 0x39 9
                   {B00000000,
                    B01100110,
                    B00000000,
                    B00000000,
                    B00000000,
                    B00000000}, // 0x3A :
                   {B00000000,
                    B11100110,
                    B00000000,
                    B00000000,
                    B00000000,
                    B00000000}, // 0x3B ;
                   {0x08, 0x14, 0x22, 0x41, 0x00, 0x00}, // 0x3C <
                   {0x14, 0x14, 0x14, 0x14, 0x14, 0x00}, // 0x3D =
                   {0x00, 0x41, 0x22, 0x14, 0x08, 0x00}, // 0x3E >
                   {0x02, 0x01, 0x51, 0x09, 0x06, 0x00}, // 0x3F ?
                   {B11111111,
                    B11111111,
                    B11111111,
                    B11111111,
                    B11111111,
                    B11111111}, // 0x40 @ // Spaceholder for fill
                   {B01111110,
                    B00000101,
                    B00000101,
                    B00000101,
                    B01111110,
                    B00000000}, // 0x41 A
                   {B01111111,
                    B01000101,
                    B01000101,
                    B01000101,
                    B00111010,
                    B00000000}, // 0x42 B
                   {B00111110,
                    B01000001,
                    B01000001,
                    B01000001,
                    B00100010,
                    B00000000}, // 0x43 C
                   {B01111111,
                    B01000001,
                    B01000001,
                    B01000001,
                    B00111110,
                    B00000000}, // 0x44 D
                   {B01111111,
                    B01001001,
                    B01001001,
                    B01000001,
                    B01000001,
                    B00000000}, // 0x45 E
                   {B01111111,
                    B00001001,
                    B00001001,
                    B00000001,
                    B00000001,
                    B00000000}, // 0x46 F
                   {B00111110,
                    B01000001,
                    B01000101,
                    B01000101,
                    B00111101,
                    B00000000}, // 0x47 G
                   {B01111111,
                    B00000100,
                    B00000100,
                    B00000100,
                    B01111111,
                    B00000000}, // 0x48 H
                   {B00000000,
                    B01000001,
                    B01111111,
                    B01000001,
                    B00000000,
                    B00000000}, // 0x49 I
                   {B00100000,
                    B01000000,
                    B01000000,
                    B01000000,
                    B00111111,
                    B00000000}, // 0x4A J
                   {B01111111,
                    B00000100,
                    B00000100,
                    B00001010,
                    B01110001,
                    B00000000}, // 0x4B K
                   {B01111111,
                    B01000000,
                    B01000000,
                    B01000000,
                    B01000000,
                    B00000000}, // 0x4C L
                   {B01111111,
                    B00000010,
                    B00000100,
                    B00000010,
                    B01111111,
                    B00000000}, // 0x4D M
                   {B01111111,
                    B00000010,
                    B00000100,
                    B00001000,
                    B01111111,
                    B00000000}, // 0x4E N
                   {B00111110,
                    B01000001,
                    B01000001,
                    B01000001,
                    B00111110,
                    B00000000}, // 0x4F O
                   {B01111111,
                    B00000101,
                    B00000101,
                    B00000101,
                    B00000010,
                    B00000000}, // 0x50 P
                   {B00111110,
                    B01000001,
                    B01000001,
                    B00100001,
                    B01011110,
                    B00000000}, // 0x51 Q
                   {B01111111,
                    B00000101,
                    B00000101,
                    B00000101,
                    B01111010,
                    B00000000}, // 0x52 R
                   {B00100010,
                    B01000101,
                    B01000101,
                    B01000101,
                    B00111001,
                    B00000000}, // 0x53 S
                   {B00000001,
                    B00000001,
                    B01111111,
                    B00000001,
                    B00000001,
                    B00000000}, // 0x54 T
                   {B00111111,
                    B01000000,
                    B01000000,
                    B01000000,
                    B00111111,
                    B00000000}, // 0x55 U
                   {B00001111,
                    B00110000,
                    B01000000,
                    B00110000,
                    B00001111,
                    B00000000}, // 0x56 V
                   {B01111111,
                    B00100000,
                    B00010000,
                    B00100000,
                    B01111111,
                    B00000000}, // 0x57 W
                   {B01110001,
                    B00001010,
                    B00000100,
                    B00001010,
                    B01110001,
                    B00000000}, // 0x58 X
                   {B00000001,
                    B00000010,
                    B01111100,
                    B00000010,
                    B00000001,
                    B00000000}, // 0x59 Y
                   {B01100001,
                    B01010001,
                    B01001001,
                    B01000101,
                    B01000011,
                    B00000000}, // 0x5A Z
                   {0x00, 0x7f, 0x41, 0x41, 0x00, 0x00}, // 0x5B [
                   {0x02, 0x04, 0x08, 0x10, 0x20, 0x00}, // 0x5C Y
                   {0x00, 0x41, 0x41, 0x7f, 0x00, 0x00}, // 0x5D ]
                   {0x04, 0x02, 0x01, 0x02, 0x04, 0x00}, // 0x5E ^
                   {B10000000,
                    B10000000,
                    B10000000,
                    B10000000,
                    B10000000,
                    B00000000}, // 0x5F _
                   {0x00, 0x01, 0x02, 0x04, 0x00, 0x00}, // 0x60 `
                   {B00100000,
                   B01010100,
                   B01010100,
                   B01010100,
                   B01111000,
                   B00000000}, // 0x61 a
                   {B01111111,
                   B01001000,
                   B01000100,
                   B01000100,
                   B00111000,
                   B00000000}, // 0x62 b
                   {B00111000,
                   B01000100,
                   B01000100,
                   B01000100,
                   B00101000,
                   B00000000}, // 0x63 c
                   {B00111000,
                   B01000100,
                   B01000100,
                   B01001000,
                   B01111111,
                   B00000000}, // 0x64 d
                   {B00111000,
                   B01010100,
                   B01010100,
                   B01010100,
                   B01011000,
                   B00000000}, // 0x65 e
                   {B00000000,
                   B00000100,
                   B01111110,
                   B00000101,
                   B00000101,
                   B00000000}, // 0x66 f
                   {B10011000,
                   B10100100,
                   B10100100,
                   B10100100,
                   B01111100,
                   B00000000}, // 0x67 g
                   {B01111111,
                   B00001000,
                   B00000100,
                   B00000100,
                   B01111000,
                   B00000000}, // 0x68 h
                   {B00000000,
                   B00000000,
                   B01111101,
                   B00000000,
                   B00000000,
                   B00000000}, // 0x69 i
                   {B01100000,
                   B10000000,
                   B10000000,
                   B10000000,
                   B01111101,
                   B00000000}, // 0x6A j
                   {B01111111,
                   B00010000,
                   B00101000,
                   B01000100,
                   B00000000,
                   B00000000}, // 0x6B k
                   {B00000000,
                   B00000000,
                   B00111111,
                   B01000000,
                   B00000000,
                   B00000000}, // 0x6C l
                   {B01111100,
                   B00000100,
                   B00011000,
                   B00000100,
                   B01111000,
                   B00000000}, // 0x6D m
                   {B01111100,
                   B00000100,
                   B00000100,
                   B00000100,
                   B01111000,
                   B00000000}, // 0x6E n
                   {B00111000,
                   B01000100,
                   B01000100,
                   B01000100,
                   B00111000,
                   B00000000}, // 0x6F o
                   {B11111100,
                   B00101000,
                   B00100100,
                   B00100100,
                   B00011000,
                   B00000000}, // 0x70 p
                   {B00011000,
                   B00100100,
                   B00100100,
                   B00101000,
                   B11111100,
                   B00000000}, // 0x71 q
                   {B01111100,
                   B00001000,
                   B00000100,
                   B00000100,
                   B00001000,
                   B00000000}, // 0x72 r
                   {B01001000,
                   B01010100,
                   B01010100,
                   B01010100,
                   B00100100,
                   B00000000}, // 0x73 s
                   {B00000000,
                   B00000010,
                   B00111111,
                   B01000010,
                   B00000000,
                   B00000000}, // 0x74 t
                   {B00111100,
                   B01000000,
                   B01000000,
                   B01000000,
                   B01111100,
                   B00000000}, // 0x75 u
                   {B00011100,
                   B00100000,
                   B01000000,
                   B00100000,
                   B00011100,
                   B00000000}, // 0x76 v
                   {B00111100,
                   B01000000,
                   B01110000,
                   B01000000,
                   B01111100,
                   B00000000}, // 0x77 w
                   {B01000100,
                   B00101000,
                   B00010000,
                   B00101000,
                   B01000100,
                   B00000000}, // 0x78 x
                   {B10011100,
                   B10100000,
                   B10100000,
                   B10100000,
                   B01111100,
                   B00000000}, // 0x79 y
                   {B01000100,
                   B01100100,
                   B01010100,
                   B01001100,
                   B01000100,
                   B00000000}  // 0x7A z
						};

// function for clear all pixel
void lcdclear()
{
  for(byte i = 0x60; i <= 0x64; i++) // for each line
  {
    for(byte j = 0; j <= 0x60; j++) // for each column
    {
      Wire.beginTransmission(LCD_ADDR); // start transmition to device
      Wire.write(i);                // send first byte for addressing line
      Wire.write(j);                // send second byte for addressing column
      Wire.write(0x00);             // send third byte for bitmap
      Wire.endTransmission();       // stop transmition
    }
  }
}

// function for clear a given line from given column to end of line
void lcdclearln(int x, int y)
{
  for(byte j = x; j <= 0x60; j++) // for each column in line i
  {
    Wire.beginTransmission(LCD_ADDR); // start transmition to device
    Wire.write(y);                // send first byte for addressing line
    Wire.write(j);                // send second byte for addressing column
    Wire.write(0x00);             // send third byte for bitmap
    Wire.endTransmission();       // stop transmition
  }
}

// function needed for right bounded text to overwrite the unused old pixel
String rightboundtext(int maxlen, String text)
{
  // spacer      number  border
  // O   O   O   1   2   |        5 chars reserved, 2 used, 3 spacer
  //
  // spacer  number      border
  // O   O   1   1   2   |        5 chars reserved, 3 used, 2 spacer
  
  int spacecount = maxlen - text.length(); // calculate amount of spacer needed in front
  char space[spacecount] = "";
  for(int i = 0; i <= spacecount; i++)
  {
    strcat(space, " ");
  }
  String output = String(space + text); // bind spacer and text together

  return output; // return formated text
}

// function for set position to draw next
void setCursorPos(int x, int y)
{
  column = x;
  line = 0x60 + y;
}

// function for print a given string to display
void lcdprint(String text)
{
  for(int i = 0; i < text.length(); i++) // for each character
  { 
    if((0x60 - column) < 5) // check if end of display column is reached. otherwise maybe only half of the char can be drawn
    {
      line++;        // continue in next line
      column = 0;
    }
    
    int index = text[i] - 0x20; // font array beginns with 0x20. substract 0x20 to calculate the index of the character bitmap in the font array
  	// draw every character by his bitmap
  	for(int j = 0; j < sizeof(font[index]); j++)
  	{
  	  draw(font[index][j]);
  	}
  }
}

// function for send bytes to display with i2c
void draw(byte bitmap)
{
  Wire.beginTransmission(LCD_ADDR); // start transmition to device
  Wire.write(line);                    // send first byte for addressing line
  Wire.write(column);                    // send second byte for addressing column
  column++;                         // increse column count
  Wire.write(bitmap);               // send third byte for bitmap
  Wire.endTransmission();           // stop transmition
}
