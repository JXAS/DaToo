// datoo.ino : Main programm
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

int looptime;
int m,n;
char conf[250];

//char TMhex[2*sizeof(my_TM)];
char TMhex[500];
char TMXbee_str[NB_XBEE*sizeof(my_TM.TMX)];

int init_TM (void)
{

   my_stream.version=RELEASE + VERSION*256;
   my_stream.xbee_detected=15;
   my_stream.ios=0x0589;
   my_TM.counter=0;
   my_TM.fail=0;
   my_TM.rssi=0; //seront generes par AT+CSQ
   my_TM.ber=0;
   my_TM.scan_period=looptime;
   my_TM.here=0;
   my_TM.portin=0;

   for (n=0;n<NB_XBEE;n++)
   {
	   strncpy(my_xbee_module[n].module_id,conf+16+(12*n),8);
      my_xbee_module[n].module_id[8]=0;
	   my_xbee_module[n].local_temp=0;
	   my_xbee_module[n].analog_input=0;
	   my_xbee_module[n].battery=0;
	   my_xbee_module[n].xbee_alarm_count=0;
      strcpy(my_TM.TMX[n].temp_local,"0000");
      strcpy(my_TM.TMX[n].analog2,"0000");
      strcpy(my_TM.TMX[n].battery,"0000");
      strcpy(my_TM.TMX[n].ios,"0000");
      my_TM.TMX[n].alarm_hit=0;
   }

return 0;
}

void DisplayDatas (void)
{
   unsigned int mask[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,\
16384,32768};
char buf[5],config[5];
   int result,*intptr,intvalue;
   long retard;

   //XBee vars
   char xbeestr[2048];
   int longueur, x1,x2,x3,test;

   char buffer[6];

   buf[4]=0x00;
   config[4]=0x00;
   strncpy(config, &conf[12],4);
   result=numhexstr2bin (config, buf, 2 );
   intptr= (int*)buf;
   nb_item=*intptr;
   strncpy(config, &conf[8],4);
   result=numhexstr2bin (config, buf, 4 );
   intptr= (int*)buf;
   looptime=*intptr;

   for (n=0;n<nb_item;n++)
   {
	   strncpy(config, conf+24+n*12,4);
	   result=numhexstr2bin (config, buf, 2 );
	   intptr= (int*)buf;
      intvalue=*intptr;
   	my_xbee_module[n].conf= intvalue;
//	   printf("config=%s intvalue=%u\n",config,intvalue);
   }
   printf("\nID : %s\n",my_stream.id);
   printf("Version : %i.%i\n",(my_stream.version&0xFF00)>>8, my_stream.version&0x00FF);
   printf("XBee-Detected : %04X\n",my_stream.xbee_detected);
   printf("In's : %02X\n",(my_stream.ios & 0xff00)>>8);
   printf("Out's : %02X\n\n",my_stream.ios & 0x00ff);

   printf("NbItems=%u\n",nb_item);
	for (n=0;n<nb_item;n++)
   {
	   printf("Module #%u\n\tID : %s Conf=%u\n",n,\
      my_xbee_module[n].module_id,my_xbee_module[n].conf);
		if ((my_xbee_module[n].conf)&mask[0])
	      printf("\tLocalTemp : %i\n",my_xbee_module[n].local_temp);
		if ((my_xbee_module[n].conf)&mask[1])
	      printf("\tAnalogInput : %i\n",my_xbee_module[n].analog_input);
		if ((my_xbee_module[n].conf)&mask[2])
	      printf("\tXBeeBattery : %i\n",my_xbee_module[n].battery);
		if ((my_xbee_module[n].conf)&mask[3])
	      printf("\tPower-230VAC #1\n");
		if ((my_xbee_module[n].conf)&mask[4])
	      printf("\tPower-230VAC #2\n");
		if ((my_xbee_module[n].conf)&mask[5])
	      printf("\tSprinkle #1\n");
		if ((my_xbee_module[n].conf)&mask[6])
	      printf("\tSprinkle #2\n\n");
      if ((my_xbee_module[n].conf)&mask[7])
      {
         printf("\tXBeeAlarmCount : %i\n",my_xbee_module[n].xbee_alarm_count);
			xbee_alarm[nb_xbee_alarm]=n;
         nb_xbee_alarm++;
      }
   }
   printf("Nombre de XBee Alarme=%u\n",nb_xbee_alarm);

   for (n=0;n<nb_xbee_alarm;n++)
   {
   	alarm_delay[xbee_alarm[n]]=SEC_TIMER;
   	my_xbee_module[n].module_id[8]=0;
   	printf("%u : %s\n",n,my_xbee_module[xbee_alarm[n]].module_id);
   }


}


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

void ParseCommand (void)
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
