// morse-decode-encode
// by Oompah
// March 3 2023
// credit to "Programming Arduino" by Simon Monk, (c)2016
// for some of the code lines
//
// Either Key in a Morse code sequence using the button to decode,
// or click in the field in the Serial Monitor, and
// type letters followed by Enter to encode into Morse code.
// The LED and tone will sound while you key in the sequence.
// This sketch decodes sequences with letters or numbers
// and prints them to the serial monitor.
// A beep sounds and "?" prints for unrecognized sequences.
//
// To learn Morse Code, type a word in the Serial Monitor,
// Then try keying in the same word as you heard it.
// Tips: 
//   Keep dots short, and dashes long. 
//   Leave a short space after letters.
//   Leave a longer space after words.
//   Try to make nice clean presses and releases.
//
// Circuit:
// Piezo buzzer or speaker connects to pin 2 and ground
// Button connects between pin 8 and ground
// LED (+) to pin 13, (-) 220 Ohm resistor to ground
//
#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 3
#define DESTINATION_ADDRESS_1 0

RF22Router rf22(MY_ADDRESS); // Initiate the RF22 with the sender's address


int tonePin = 2;
int toneFreq = 1000;
int ledPin = 13;
int buttonPin = 8;
int debounceDelay = 90;

int dotLength = 240; 
// dotLength = basic unit of speed in milliseconds
// 240 gives 5 words per minute (WPM) speed.
// WPM = 1200/dotLength.
// For other speeds, use dotLength = 1200/(WPM)
//
// Other lengths are computed from dot length
  int dotSpace = dotLength;
  int dashLength = dotLength*4;
  int letterSpace = dotLength*3;
  int wordSpace = dotLength*7; 
  float wpm = 1200./dotLength;
  
int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound, keyboardText;
int lineLength = 0;
int maxLineLength = 20; 

char* letters[] = 
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* numbers[] = 
{
"-----", ".----", "..---", "...--", "....-", //0-4
".....", "-....", "--...", "---..", "----." //5-9
};

String dashSeq = "";
char keyLetter, ch;
int i, index;
String decodedLetters = "";  // Holds all decoded letters
int number_of_bytes=0; // will be needed to measure bytes of message

float throughput=0; // will be needed for measuring throughput
int flag_measurement=0;

int counter=0;
int initial_time=0;
int final_time=0;
 bool k=true;
 
void setup() 
{
 

  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Morse Code decoder/encoder");
  Serial.print("Speed=");
  Serial.print(wpm);
  Serial.print("wpm, ");
  Serial.print("dot=");
  Serial.print(dotLength);
  Serial.println("ms");   

  
  // Rx,Tx code
  if (!rf22.init()) // initialize my radio
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(434.0)) // set the desired frequency
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM); // set the desired power for my transmitter in dBm
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::OOK_Rb40Bw335  ); // set the desired modulation
  //modulation

  // Manually define the routes for this network
  rf22.addRouteTo(DESTINATION_ADDRESS_1, DESTINATION_ADDRESS_1); // tells my radio card that if I want to send data to DESTINATION_ADDRESS_1 then I will send them directly to DESTINATION_ADDRESS_1 and not to another radio who would act as a relay
  delay(1000); // delay for 1 s

  


 // for(int pinNumber = 4; pinNumber<6; pinNumber++) // I can use pins 4 to 6 as digital outputs (in the example to turn on/off LEDs that show my status)
//  {
  //  pinMode(pinNumber,OUTPUT);
//    digitalWrite(pinNumber, LOW);
//  }
  delay(1000); // delay for 1 s

// Test the LED and tone
  tone(tonePin, toneFreq);
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(600);

//flash to demonstrate the expected key speed
//A
   Serial.print("A .-  ");
   index = 'A'-65;
   flashSequence(letters[index]);
   delay(wordSpace);
//B
 //  Serial.print("B -...  ");
 //  index = 'B'-65;
 //  flashSequence(letters[index]);
  // delay(wordSpace);
//C
 //  Serial.print("C -.-.  ");
  // index = 'C'-65;
  // flashSequence(letters[index]);
  // delay(wordSpace);

  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Click field in Serial Monitor,");
  Serial.println("type text and press Enter, or");
  Serial.println("Key in Morse Code to decode:");
  Serial.println("-------------------------------");
      
  newLetter = false; //if false, do NOT check for end of letter gap
  newWord = false;  //if false, do NOT check for end of word gap
  keyboardText = false; 
}

void loop() 
{//Rx code receive code from center arduino 







  Serial.println("ΗΙ");
  delay(1000);

while(k==true )
{    
      Serial.println("VCVC");

     delay(1000);

  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN]; // Buffer to hold incoming data
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN]; // Buffer to hold converted incoming data as a string
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf); // Length of the incoming data
  uint8_t from; // Variable to store the sender's address
  // Check if data is received

 Serial.println(incoming);

  if (rf22.recvfromAck(buf, &len, &from)) {
    buf[len] = '\0'; // Ensure null-termination for proper string handling
    memcpy(incoming, buf, len + 1); // Copy received data into incoming buffer, ensuring it's a valid string

    Serial.print("Message received from address: ");
    Serial.println(from, DEC); // Display the sender's address
    Serial.print("Message: ");
    Serial.println(incoming); // Display the received message as a string
    delay(1000);
    if(incoming=="Arduino 3 get start"){
      k=false;}
  }
}


  
// Check to see if something has been entered on the keyboard

  
if (Serial.available() > 0)
  {
    if (keyboardText == false) 
    {
      Serial.println();
      Serial.println("-------------------------------");
    }
    keyboardText = true;
    ch = Serial.read();
    if (ch >= 'a' && ch <= 'z')
    { ch = ch-32; }
    
    if (ch >= 'A' && ch <= 'Z')
    {
      Serial.print(ch); 
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')
    {
      Serial.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }
    if (ch == ' ')
    {
      Serial.println("_");
      delay(wordSpace);    
    } 

// Print a header after last keyboard text    
     if (Serial.available() <= 0) 
     {
      Serial.println();
      Serial.println("Enter text or Key in:");
      Serial.println("-------------------------------");
      keyboardText = false;
     }
  }
 
  if (digitalRead(buttonPin) == HIGH  ) //button is pressed
  {
    newLetter = true; 
    newWord = true;
    t1 = millis(); //time at button press
    digitalWrite(ledPin, HIGH); //turn on LED and tone
    tone(tonePin, toneFreq);
    delay(debounceDelay);     
    while (digitalRead(buttonPin) == HIGH ) // wait for button release
    {
      delay(debounceDelay);
    }
    delay(debounceDelay);
       
    t2 = millis();  //time at button release
    onTime = t2 - t1;  //length of dot or dash keyed in
    digitalWrite(ledPin, LOW); //turn off LED and tone
    noTone(tonePin); 
    
    //check if dot or dash 

    if (onTime <= dotLength * 1.5) //allow for 50% longer 
      {dashSeq += ".";} //build dot/dash sequence
    else 
      {dashSeq += "-";}
  }  //end button press section
  
// look for a gap >= letterSpace to signal end letter
// end of letter when gap >= letterSpace

  gap=millis()-t2; 
  if (newLetter && gap >= letterSpace) {
    letterFound = false;
    keyLetter = '?'; // Default unknown letter

    // Search for matching letter in Morse sequence array
    for (int i = 0; i < 26; i++) {
      if (dashSeq == letters[i]) {
        keyLetter = char(i + 65); // Convert index to ASCII character
        letterFound = true;
        break;
      }
    }

    // Now check numbers if no letter was found
    if (!letterFound) {
      for (int i = 0; i < 10; i++) {
        if (dashSeq == numbers[i]) {
          keyLetter = char(i + 48); // Convert index to ASCII number
          letterFound = true;
          break;
        }
      }
    }
    // Output the found character
    Serial.print(keyLetter);
    decodedLetters += keyLetter; // Append this letter to the sequence

    // Reset for next input
    newLetter = false;
    dashSeq = "";

    // Check if the sequence "ABC" is found
    if (decodedLetters.endsWith("AE")) {
      Serial.println(" Success");
      decodedLetters = ""; // Optionally reset the decoded sequence

      counter = 0;
      initial_time = millis();

      // Replace sensorVal with a specific message
      char message[] = "Experiment 1 Finish"; // Message to be sent

      Serial.print("Sending message: ");
      Serial.println(message); // Show the message on Serial Monitor

      uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN]; // Buffer for sending data
      memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);    
      strncpy((char *)data_send, message, RF22_ROUTER_MAX_MESSAGE_LEN - 1); // Copy the message to the data_send buffer

      number_of_bytes = strlen((char *)data_send); // Calculate the number of bytes of the message
      Serial.print("Number of Bytes= ");
      Serial.println(number_of_bytes); // Display the number of bytes on the monitor

      // Sending the message to DESTINATION_ADDRESS_1
      if (rf22.sendtoWait(data_send, number_of_bytes, DESTINATION_ADDRESS_1) != RF22_ROUTER_ERROR_NONE) // Sending the data
      {
        Serial.println("sendtoWait failed"); // Report failed transmission
      }
      else
      {
        counter = counter + 1;
        Serial.println("sendtoWait Successful"); // Report successful transmission
      }
      Serial.println("after");
    }
  }  
  
  
// keyed letter has been identified and printed

// when gap is >= wordSpace, insert space between words
// lengthen the word space by 50% to allow for variation

  if (newWord == true && gap>=wordSpace*3)
    { 
     newWord = false; 
     Serial.print("_");  
     lineLength=lineLength+1;
     
// flash to indicate new word

    digitalWrite(ledPin, HIGH);
    delay(25);
    digitalWrite(ledPin, LOW);       
    } 

// insert linebreaks

  if (lineLength >= maxLineLength) 
    {
      Serial.println();
      lineLength = 0;
    }      
} 

void flashSequence(char* sequence)
{
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-')
  {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash)
{
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.')
   { delay(dotLength); }
     else
   { delay(dashLength); }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength); 
}
//--- end of sketch ---
