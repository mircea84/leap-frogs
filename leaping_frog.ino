//Pin connected to ST_CP of 74HC595
int latchPin = 40;  //8
//Pin connected to SH_CP of 74HC595
int clockPin = 41;  //12
////Pin connected to DS of 74HC595
int dataPin = 42;  //11

// buttons
// you may use a trick to control more buttons with only 3 pins
// check here http://www.instructables.com/id/How-to-access-5-buttons-through-1-Arduino-input/?ALLSTEPS
const int buttonPin0 = 30;  //0
const int buttonPin1 = 31;  //1
const int buttonPin2 = 32;  //2
const int buttonPin3 = 33;  //3
const int buttonPin4 = 34;  //4
const int buttonPin5 = 35;  //5
const int buttonPin6 = 36;  //6

//holds the status array
int fGreen[7] = {0,0,0,0,1,1,1};
int fRed[7] = {1,1,1,0,0,0,0};

//holders for infromation you're going to pass to shifting function
byte data_fGreen;
byte data_fRed;



void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(buttonPin0, INPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  pinMode(buttonPin5, INPUT);
  pinMode(buttonPin6, INPUT);
  //Serial.begin(9600);
  //fGreen = {0,0,0,0,1,1,1};
  //fRed = {1,1,1,0,0,0,0};
}

//function to move the GREEN frog to left
void moveR2L (int j) {
  if ( j > 0 ) {
    if ( fGreen[j-1] == 0 && fRed[j-1] == 0 ) {
      fGreen[j] = 0;
      fGreen[j-1] = 1;
    }
    if (( fGreen[j-1] == 1 || fRed[j-1] == 1 ) && ( fGreen[j-2] == 0 || fRed[j-2] == 0 )) {
       fGreen[j] = 0;
       fGreen[j-2] = 1;
    }
  }
}

// function to move the RED frog to right
void moveL2R (int j){
  if ( j < 6 ) {
    if ( fRed[j+1] == 0 && fGreen[j+1] == 0 ) {
      fRed[j] = 0;
      fRed[j+1] = 1;
    }
    if (( fGreen[j+1] == 1 || fRed[j+1] == 1 ) && ( fGreen[j+2] == 0 || fRed[j+2] == 0 )) {
       fRed[j] = 0;
       fRed[j+2] = 1;
    }
  }
}

// function to check which button was pressed
int check_pressed(){
  //  for (int i = 0; i < 7; i++) {
  int button = 7;
  
  while (button == 7) {
    if (digitalRead(buttonPin0) == HIGH) { button = 0; }
    if (digitalRead(buttonPin1) == HIGH) { button = 1; }
    if (digitalRead(buttonPin2) == HIGH) { button = 2; }
    if (digitalRead(buttonPin3) == HIGH) { button = 3; }
    if (digitalRead(buttonPin4) == HIGH) { button = 4; }
    if (digitalRead(buttonPin5) == HIGH) { button = 5; }
    if (digitalRead(buttonPin6) == HIGH) { button = 6; }
  }
  return button;

}
    
byte arrayToByte(int arr[], int len){
  // Convert -1 to 0 and pack the array into a byte
  int i;
  byte result = 0;

  for(i=len-1; i>=0; i--){
    if(arr[i] == 0){
      result &= ~(0 << i);
    } else {
      result |= (1 << i);
    }
  }
  return result;
}

void loop() {
  //  code to display forg vectors on LEDs
  data_fGreen = arrayToByte( fGreen, sizeof(fGreen));  //0xF0
  data_fRed = arrayToByte( fRed, sizeof(fRed));  //0x07
  //ground latchPin and hold low for as long as you are transmitting
  digitalWrite(latchPin, 0);
  //move 'em out
  shiftOut(dataPin, clockPin, data_fGreen);
  shiftOut(dataPin, clockPin, data_fRed);
  //return the latch pin high to signal chip that it
  //no longer needs to listen for information
  digitalWrite(latchPin, 1);

  // check which button was pressed
  int button = check_pressed();
  int i = button;
  
  // check if the button that was pressed has some frog above it (the led is ON)
  if ( fGreen[i] = 1 ) { moveR2L(i); }
  if ( fRed[i] = 1 ) { moveL2R(i); }
  
  // check goal achieved
  
}




// the heart of the program
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}
