#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 2 // define my unique address
#define DESTINATION_ADDRESS_1 0 // define who I can talk to


// Singleton instance of the radio
RF22Router rf22(MY_ADDRESS); // initiate the class to talk to my radio with MY_ADDRESS


const int PHOTO_RESISTOR2_PIN=A1;
const int PHOTO_RESISTOR1_PIN=A0;
const int TRIG_PIN_RIGHT=13;
const int ECHO_PIN_RIGHT=12;
const int TRIG_PIN_LEFT=11;
const int ECHO_PIN_LEFT=10;


float duration_right,duration_left,distance_right,distance_left;
int cnt=0;
 int forceValue=0;
int lightValue=0;

void sendMessage(const char* message);
String receiveMessage();

void setup() {

  pinMode(TRIG_PIN_RIGHT, OUTPUT);
  pinMode(ECHO_PIN_RIGHT, INPUT);
  
  pinMode(TRIG_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  
  pinMode(FORCE_SENSOR_PIN, INPUT); // Configure force sensor pin as input
  
  pinMode(PHOTO_RESISTOR_PIN, INPUT); // Configure photoresistor pin as input
  
    Serial.begin(9600);
 
//Tx,Rx code 
 
if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(442.0)) // The frequency should be the same as that of the transmitter. Otherwise no communication will take place
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::OOK_Rb40Bw335  );// The modulation should be the same as that of the transmitter. Otherwise no communication will take place
  //modulation

  // Manually define the routes for this network
  rf22.addRouteTo(DESTINATION_ADDRESS_1, DESTINATION_ADDRESS_1);

   Serial.println("set up complete");
 
}



void loop() {

 String receivedMessage = receiveMessage();


  if(receivedMessage=="Experiment 2 Start" && cnt==0)
                    cnt++;
      

if(cnt==1)
{

 //for the right 
  digitalWrite(TRIG_PIN_RIGHT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_RIGHT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_RIGHT, LOW);
  
  duration_right = pulseIn(ECHO_PIN_RIGHT, HIGH);
  distance_right = (duration_right*.0343)/2;
 // Serial.print("Distance: ");
  //Serial.println(distance);
  //delay(100);
  
  
  //for the left
  digitalWrite(TRIG_PIN_LEFT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_LEFT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_LEFT, LOW);
  
  duration_left = pulseIn(ECHO_PIN_LEFT, HIGH);
  distance_left = (duration_left*.0343)/2;
  //Serial.print("Distance: ");
  //Serial.println(distance);
 // delay(100);

  lightValue1 = analogRead(PHOTO_RESISTOR1_PIN);
  lightValue2 = analogRead(PHOTO_RESISTOR2_PIN);

  bool k=false;

 /*
 sendMessage("Moving Right");
 delay(1000);
  sendMessage("Moving Left");
   delay(1000);
  sendMessage("Moving Down");
   delay(1000);
 sendMessage("Moving Up");
  delay(1000);
 sendMessage("No move");
  delay(1000);
 
 
 */
 
 

 if ( distance_right < 100) {
  // Move right
  Serial.println("Moving Right");
  sendMessage("Right");
  // Add your code to move right here
   k=true;
  delay(1000);

}

if(distance_left < 100) {
  // Move left
  Serial.println("Moving Left");
  sendMessage("Left");
  // Add your code to move left here
  k=true;
  delay(1000);
 
}

if (lightValue2 > 500) {
  // Move down
  Serial.println("Moving Down");
  sendMessage("Down");
  Serial.println(lightValue2);
  // Add your code to move down here
  k=true;
  delay(1000);
  
}

if (lightValue1 > 500) {
  // Move up
  Serial.println("Moving Up");
  sendMessage("Up");
  Serial.println(lightValue1);
  // Add your code to move up here
  k=true;
  
  delay(1000);
  
}

if (k==false) {
  // If none of the conditions are met
  Serial.println("Nothing");
  sendMessage("No move");
  // Add your code for no movement here
  delay(1000);
}
  

}//end if from incoming

}//loop


void sendMessage(const char* message) {
 /*uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memcpy(data_send, message, strlen(message));

  if (rf22.sendtoWait(data_send, strlen(message), DESTINATION_ADDRESS_1) != RF22_ROUTER_ERROR_NONE) {
    Serial.println("sendtoWait failed");
  } else {
    Serial.println("sendtoWait Successful");
  }
  
 
 */ 
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memcpy(data_send, message, strlen(message));
    
    Serial.println("Attempting to send finish message...");

    bool success = false;
    for (int attempt = 0; attempt < 5; attempt++) {  // Retry up to 3 times
        Serial.print("Attempt ");
        Serial.println(attempt + 1);
        if (rf22.sendtoWait(data_send, strlen(message),DESTINATION_ADDRESS_1) == RF22_ROUTER_ERROR_NONE) {
            Serial.println("sendtoWait Successful");
            success = true;
            break;
        } else {
            Serial.println("sendtoWait failed");
        }
        delay(1000); // Wait 1 second before retrying
    }

    if (!success) {
        Serial.println("Failed to send finish message after 3 attempts.");
    }


}



String receiveMessage() {
    uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
    char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    uint8_t len = sizeof(buf);
    uint8_t from;

    if (rf22.recvfromAck(buf, &len, &from)) {
        buf[len] = '\0';
        memcpy(incoming, buf, len + 1);

        Serial.print("Message received from address: ");
        Serial.println(from, DEC);
        Serial.print("Message: ");
        Serial.println(incoming);

        return String(incoming);
    }
    return "";
}



