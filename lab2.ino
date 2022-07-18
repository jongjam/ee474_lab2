//James Jhong 1939553
#define DDL2 1 << 2 //47
#define DDL1 1 << 1 //48
#define DDL0 1 << 0 //49
#define DDH3 1 << 3 //OCA4 output

//#define PORTH3 1 << 3
#define PORTL2 1 << 2
#define PORTL1 1 << 1
#define PORTL0 1 << 0
#define PORTH3 1 << 3;

#define LED_PIN_A 47 
#define LED_PIN_B 48
#define LED_PIN_C 49
#define d_val 333

#define TIMER4_ON_BIT PRTIM4; //Writing logic one to this shuts Timer/Counter 4 module. WHen enabled, operates like before shutdown
#define TIMER5_ON_BIT PRTIM5;
#define TIMER3_ON_BIT PRTIM3;

//SPI define calls
#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

// change these pins as necessary
int DIN = 12; //PB7
int CS =  11; //PB6
int CLK = 10; //PB5

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data

void spiTransfer(volatile byte row, volatile byte data);

void setup() {
// 1.1 Code
// pinMode(LED_PIN_A, OUTPUT);
// pinMode(LED_PIN_B, OUTPUT);
// pinMode(LED_PIN_C, OUTPUT);
  
  //For 1.4
  //Setting these pins here as outputs !
  DDRL |= DDL2; //activiating PA6, pin 47
  DDRL |= DDL0; //activating PA4, pin 49
  DDRL |= DDL1; //activating PA5, pin 48
  
   //Setting Pin 6 (timer pin) as an output
  DDRH |= DDH3;
  
  //Setting up Timer 4 (Part 2)
  noInterrupts();
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0;
  
  OCR4A = 0;//Compare match register, look up... //change these values to change the frequency.
  TCNT4 = 0;
  TCCR4A |= (1 << COM4A0); //Toggling on compare match
  TCCR4B |= (1 << WGM42); //CTC MODE
  TCCR4B |= (1 << CS40); //Prescalar 1

  //LED matrix setup
  DDRB |= (1 << DDB6); //DIN
  DDRB |= (1 << DDB5); //CS
  DDRB |= (1 << DDB4); //CLOCK
  PORTH |= (1 << PORTB5);
  
  spiTransfer(OP_DISPLAYTEST,0);
  spiTransfer(OP_SCANLIMIT,7);
  spiTransfer(OP_DECODEMODE,0);
  spiTransfer(OP_SHUTDOWN,1);

//  //Joystick setup
//  DDRF &= ~(1 << DDF0); //Brown wire, X movement PE5
//  DDRF &= ~(1 << DDF1); //Orange wire,y movement PE4
}


//This method calculates the ocr4A value based on the equation given in the hardware book.
void orc_calc(int freq) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = (16000000 / 2 * freq) - 1 ;
  }
}

void loop() {
  //Code for Part 1.1 (Running LEDs on pins 47 - 49 in sequential order for 0.333 seconds each)
  //digWriteLEDs();
  //registerLEDs();
  //timersPart2();
  int vert = analogRead(A0);
  int horiz = analogRead(A1);

  char x_move = map(horiz, 1021, 0,7,0); //The lights to be on
  char y_move = map(vert , 1021, 0,0,7);

  Serial.print("X coord:");
  Serial.print(horiz, DEC);
  Serial.print("Y coord:");
  Serial.print(vert, DEC);
}

//This task manipulates the timers to output square wave
void timersPart2() {
  //Part 2.3
  //I want to generate square wave on OC4A --> ABSTRACT PORT:PH3, HW PIN: 6, ADP: 7  (labeled as pin 6 on the board)
  //CTC mode
  //What scaler I want
  //modulus
  static int timer;
  if (timer % 4000 == 0 ){
    orc_calc(400); //its running 400 HZ
  }else if (timer % 4000 == 1) {
    orc_calc(800); //800 Hz
  } else if (timer % 4000 == 2) {
    orc_calc(250);
  } else if (timer % 4000 == 3 {
    orc_calc(0);
  }
  delay(100);
  timer++;
}

//Part 1.4 (Trying to do Part 1.1 without digitalWrite or pinMode)
    /*
    * I need PINS 47, 48, 49 from the board. 
    * 47, 48, 49 are all Port L
    * 47 : PL2 
    * 48 : PL1
    * 49 : PL0
    *
    * Setting an input would look like DDRA &= ~(1 << DDB4);
    *  
    */
//This is code functions the same as in the function digWriteLEDs but
//does not utilize digital write
void registerLEDs() {
  //Pin 47
  PORTL &= ~(PORTL2); //pin 47 low
  delay(333);
  PORTL |= PORTL2; //setting pin 47 to high
  delay(333);
  //Pin 48
  PORTL &= ~(PORTL1); //pin 47 low
  delay(333);
  PORTL |= PORTL1; //setting pin 47 to high
  delay(333);
  //Pin 49
  PORTL &= ~(PORTL0); //pin 47 low
  delay(333);
  PORTL |= PORTL0; //setting pin 47 to high
  delay(333);
}

void stick() {
  int vert = analogRead(A0);
  int horiz = analogRead(A1);

   char x_move = map(horiz, 1021, 0,7,0); //The lights to be on
  char y_move = map(vert , 1021, 0,0,7);

  Serial.print("X coord:");
  Serial.print(horiz, DEC);
  Serial.print("Y coord:");
  Serial.print(vert, DEC);
}

//This function contains the code to display on the 8x8 LED display based on movements from the joystick. 
void matrix() {
  int vert = analogRead(A0);
  int horiz = analogRead(A1);

  char x_move = map(horiz, 1021, 0,7,0); //The lights to be on
  char y_move = map(vert , 1021, 0,0,7);

  //So I have the coordinate I need...  
  //spiTransfer(x, 0b0000 (just the y));

  //just need to conver the other rows to all 0b00000000
  int x_coord = 1 << x_move;
  int y_coord = y_move;
  clearBoard();
  static int timer;
  timer++;
  if (timer % 50 == 0) {
    spiTransfer(y_coord, x_coord);
  } else if (timer % 50 == 1) {
    int i ;
    for (i = 0; i < 8; i++) { //clears the rest of the rows
      if (i != y_coord) {
        spiTransfer(i, 0b00000000); 
      }
    }
  }
  Serial.print("X coord:");
  Serial.print(x_move, DEC);
  Serial.print("Y coord:");
  Serial.print(y_move, DEC);
  
  /*
  //rest in pepperonis
  delay(500);
  int RowCounter;
  if (pressUp) {
    spiTransfer(rowCounter, 0b00000000);
    rowCounter++;
    
  } if (pressDown) {
    spiTransfer(rowCounter, 0b00000000);
    rowCounter--;
  }
  int binaryRow(?); //Ask a question about this.
  if (pressRight) {
    binaryRow >> 1;
    spiTransfer(rowCounter, binaryRow);
  }*/
}



void clearBoard() {
  int i;
  for (i = 0; i < 8; i++) { //clears the rest of the rows
    spiTransfer(i, 0b00000000);  
  }
}

//spiTransfer 
void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  spidata[offset+1] = opcode+1;
  spidata[offset] = data;
  digitalWrite(CS, LOW); //
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN,CLK,MSBFIRST,spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  digitalWrite(CS,HIGH);
}

/* Starter code for the LED display... Need to modify to include a slight timing delay and ALSO to use the ports dirrectly
 * void loop()
{
    for(int n = 0; n < 56; n++) //Send column scanning cycle data
    {
        for(int t=0;t<100;t++)  //Control data scrolling speed
        {
            for(int num=n; num < 8+n; num++)//8 columns of data sent to a dot matrix
            {                       
                shiftOut(dataPin,clockPin,MSBFIRST,data[num]); //Send column data to a dot matrix
                shiftOut(dataPin,clockPin,MSBFIRST,tab[num-n]);//Send line data to a dot matrix
                //The rising edge of the data shift
                digitalWrite(latchPin,HIGH); //Output control latch HIGH  
                PORTn |= portOfDataPin whatever it is...
                digitalWrite(latchPin,LOW);  //Output control latch LOW
            }
        }
    }
}
 */


//This is the code for part 1.1-1.2
void digWriteLEDs() {
  digitalWrite(LED_PIN_A, LOW);
  delay(d_val);
  digitalWrite(LED_PIN_A, HIGH);
  delay(d_val);
  
  //48
  digitalWrite(LED_PIN_B, LOW);
  delay(d_val);
  digitalWrite(LED_PIN_B, HIGH);
  delay(d_val);

  //48
  digitalWrite(LED_PIN_C, LOW);
  delay(d_val);
  digitalWrite(LED_PIN_C, HIGH);
  delay(d_val);
}
