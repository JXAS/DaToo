// datoo.ino
#define VERSION 3
#define RELEASE 0
#define USB_BAUD 9600 

void setup() {
 Serial.begin(USB_BAUD);
 Serial.printf("USB Baud Set to %u bauds\n",USB_BAUD);
 Serial.printf("DATOO Version : %u.%u\n",VERSION,RELEASE);

}

void loop() {
  // put your main code here, to run repeatedly:

}
