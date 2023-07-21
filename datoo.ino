// datoo.ino
// by R.C.Schmidt (c)2023

#define VERSION 3
#define RELEASE 1
#define USB_BAUD 9600

#define SIGNATURE  0x22091959

#define NB_XBEE 16

#define SER_TIMEOUT 200
#define SCAN_TIMER_TO 60
#define XBEE_TIMER_TO 80000

#define SPRINKLER_EV 60

#define ALARM_STARTING 60

#define XTO 60000
#define TC_SIZE 16

struct TMXbee
{
   char temp_local[5]; //st analog input (with a temp sensor)
   char analog2[5];   // Second analog inpiut
   char battery[5];  // Power supply voltage
   int alarm_hit;
   char ios[5]; // Etat des pins dig du Xbee
};

struct TM {
	long ID;
	long counter;
	long fail;
	int here;
   char rssi;
   char ber;

	unsigned int scan_period;

	int portin;
	int portout;
	int status;

	char sprinkler1_when;
	char sprinkler1_duration;
	int sprinkler1_repetition;

	char sprinkler2_when;
	char sprinkler2_duration;
	int sprinkler2_repetition;

	struct TMXbee TMX[NB_XBEE];
};

struct XBee {
	char type[3];
   char machigh[9];
   char maclow[9];
   char sender[5];
   char options[3];
   char nbsample[3];
   char digmask[5];
   char anamask[3];
   char digsample[5];
   char temp_local[5];
   char analog2[5];
   char battery[5];
};

int nb_xbee_alarm=0;
// Contient les indices des XBEE dÈclares qui ont une alarme.
int xbee_alarm[NB_XBEE];
unsigned long start_alarme,alarm_delay[NB_XBEE];//={0,0,0,0,0,0,0,0};

struct xbee_module {
   int conf;
   	// bit 0 local temp
      // bit 1 analog input
      // bit 2 alarm
      // bit 3 Battery
      // bit 4 out-230VAC-1
      // bit 5 out-230VAC-2
      // bit 6 Sprinkle-1
      // bit 7 Sprinkle-2


	char module_id[9];
	int local_temp;
   int analog_input;
   int battery;
   int xbee_alarm_count;
};
struct sprinkle {
	int when;	// Heure de dÈmarrage (nb mn avant dÈmarrage)
   int during;	// duree d'arrosage
   int every;	// Frequence d'arrosage

   int step; //0: stop, 1:, started, 2:opened, 3:closed
	long sprinkler_counter;

};

struct stream {
	char id[9];
   int version;
   int xbee_detected;
   int ios;
};

struct xbee_module my_xbee_module[NB_XBEE];
struct stream my_stream;
struct sprinkle my_sprinkle[2];
struct TM my_TM;
struct XBee X;

//char TMhex[2*sizeof(my_TM)];
char TMhex[500];
char TMXbee_str[NB_XBEE*sizeof(my_TM.TMX)];

void GetCommandFromWeb (void)
{
}

void SetTMString (void)
{
}

void GetXbeeDatas (void)
{
}

void GetDateFromWeb (void)
{
}

void ParseCommand (void
{
}


void setup() {
 Serial.begin(USB_BAUD);
 Serial.printf("USB Baud Set to %u bauds\n",USB_BAUD);
 Serial.printf("DATOO Version : %u.%u\n",VERSION,RELEASE);

}

void loop() {
  // put your main code here, to run repeatedly:

}
