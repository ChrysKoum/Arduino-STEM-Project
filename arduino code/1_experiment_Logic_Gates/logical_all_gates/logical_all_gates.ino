#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 1 // define my unique address
#define DESTINATION_ADDRESS_1 0 // define who I can talk to

// Singleton instance of the radio
RF22Router rf22(MY_ADDRESS); // initiate the class to talk to my radio with MY_ADDRESS

// constants won't change. They're used here to set pin numbers:
const int button1Pin = 12; // the number of the pushbutton pin
const int button2Pin = 3; // the number of the pushbutton pin
const int OutputLedPin = 7; // the number of the Output LED pin
const int NotGate = 4;
const int OrGate = 5;
const int AndGate = 6;
const int NorGate = 13;
const int NandGate = 8;
const int XorGate = 9;
const int XnorGate = 10;

// variables will change:
int button1State = 0; // variable for reading the pushbutton status
int button2State = 0;
int cnt = 0; // counter for condition checks

void sendMessage(const char* message);
String receiveMessage();

// Define conditions for each gate
bool orConditions[4][3] = {
    {LOW, LOW, LOW}, 
    {LOW, HIGH, HIGH}, 
    {HIGH, LOW, HIGH}, 
    {HIGH, HIGH, HIGH}
};

bool andConditions[4][3] = {
    {LOW, LOW, LOW}, 
    {LOW, HIGH, LOW}, 
    {HIGH, LOW, LOW}, 
    {HIGH, HIGH, HIGH}
};

bool norConditions[4][3] = {
    {LOW, LOW, HIGH}, 
    {LOW, HIGH, LOW}, 
    {HIGH, LOW, LOW}, 
    {HIGH, HIGH, LOW}
};

bool nandConditions[4][3] = {
    {LOW, LOW, HIGH}, 
    {LOW, HIGH, HIGH}, 
    {HIGH, LOW, HIGH}, 
    {HIGH, HIGH, LOW}
};

bool xorConditions[4][3] = {
    {LOW, LOW, LOW}, 
    {LOW, HIGH, HIGH}, 
    {HIGH, LOW, HIGH}, 
    {HIGH, HIGH, LOW}
};

bool xnorConditions[4][3] = {
    {LOW, LOW, HIGH}, 
    {LOW, HIGH, LOW}, 
    {HIGH, LOW, LOW}, 
    {HIGH, HIGH, HIGH}
};

bool notConditions[2][2] = {
    {LOW, HIGH}, 
    {HIGH, LOW}
};

void setup() {
  pinMode(OutputLedPin, OUTPUT);
  for (int i = 4; i <= 10; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  Serial.begin(9600);

  if (!rf22.init()) {
    Serial.println("RF22 init failed");
  }

  if (!rf22.setFrequency(434.0)) {
    Serial.println("setFrequency Fail");
  }

  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::OOK_Rb40Bw335);

  rf22.addRouteTo(DESTINATION_ADDRESS_1, DESTINATION_ADDRESS_1);

  Serial.println("setup complete");
}

void loop() {
  String receivedMessage = receiveMessage();

  // Reset all gate LEDs to LOW
  for (int i = 4; i <= 10; i++) {
    digitalWrite(i, LOW);
  }

  if (receivedMessage == "OrGate") {
    digitalWrite(OrGate, HIGH);
    checkConditions(orConditions, 4);
  } else if (receivedMessage == "AndGate") {
    digitalWrite(AndGate, HIGH);
    checkConditions(andConditions, 4);
  } else if (receivedMessage == "NorGate") {
    digitalWrite(NorGate, HIGH);
    checkConditions(norConditions, 4);
  } else if (receivedMessage == "NandGate") {
    digitalWrite(NandGate, HIGH);
    checkConditions(nandConditions, 4);
  } else if (receivedMessage == "XorGate") {
    digitalWrite(XorGate, HIGH);
    checkConditions(xorConditions, 4);
  } else if (receivedMessage == "XnorGate") {
    digitalWrite(XnorGate, HIGH);
    checkConditions(xnorConditions, 4);
  } else if (receivedMessage == "NotGate") {
    digitalWrite(NotGate, HIGH);
    checkNotConditions(notConditions, 2);
  }
}

void checkConditions(bool conditions[][3], int size) {
  cnt = 0;
  while (true) {
    for (int i = 0; i < size; i++) {
      Serial.println("YOU HAVE TO DO THE FOLLOW COMBINATION:");
      Serial.print("Button1: ");
      Serial.print(conditions[i][0]);
      Serial.print(" Button2: ");
      Serial.print(conditions[i][1]);
      Serial.print(" LED: ");
      Serial.println(conditions[i][2]);

      while (i == cnt) {
        digitalWrite(OutputLedPin, LOW);
        button1State = digitalRead(button1Pin);
        button2State = digitalRead(button2Pin);
        //Serial.println(button1State);

        if (button1State == conditions[i][0] && button2State == conditions[i][1]) {
          digitalWrite(OutputLedPin, conditions[i][2]);
          delay(1000);
          Serial.println("Success");

          cnt++;
          break;
        }
      }
    }

    if (cnt == size) {
      sendMessage("finish");
        break;
    }
  }
}

void checkNotConditions(bool conditions[][2], int size) {
  cnt = 0;
  while (true) {
    for (int i = 0; i < size; i++) {
      Serial.println("YOU HAVE TO DO THE FOLLOW COMBINATION:");
      Serial.print("Button1: ");
      Serial.print(conditions[i][0]);
      Serial.print(" LED: ");
      Serial.println(conditions[i][1]);

      while (i == cnt) {
      digitalWrite(OutputLedPin, HIGH);
        button1State = digitalRead(button1Pin);

        if (button1State == conditions[i][0]) {
          digitalWrite(OutputLedPin, conditions[i][1]);
          delay(1000);
          Serial.println("Success");

          cnt++;
          break;
        }
      }
    }

    if (cnt == size) {
      sendMessage("finish");
        break;
    }
  }
}

void sendMessage(const char* message) {
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memcpy(data_send, message, strlen(message));

  if (rf22.sendtoWait(data_send, strlen(message), DESTINATION_ADDRESS_1) != RF22_ROUTER_ERROR_NONE) {
    Serial.println("sendtoWait failed");
  } else {
    Serial.println("sendtoWait Successful");
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