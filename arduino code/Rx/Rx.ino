#include <SPI.h>
#include <RF22.h>
#include <RF22Router.h>

#define MY_ADDRESS 0
#define DESTINATION_ADDRESS_1 1

RF22Router rf22(MY_ADDRESS);

void setup() {


   Serial.begin(9600);
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

}

void loop() {

 char message[] = "finish experiment";
     uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
     memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);    
     memcpy(data_send, message, strlen(message));

   if (rf22.sendtoWait(data_send, strlen(message), DESTINATION_ADDRESS_1) != RF22_ROUTER_ERROR_NONE) {
     Serial.println("sendtoWait failed");
   }
   else {
    Serial.println("sendtoWait Successful");
   }
   delay(1000);
  

}