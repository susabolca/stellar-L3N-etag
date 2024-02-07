#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Ap_29demo.h"

const char *ssid = "MI5S";
const char *ssid_pswd = "zhaolei123";

int BUSY_Pin = D0; 
int RES_Pin = D1; 
int DC_Pin = D2; 
int CS_Pin = D4; 
int SCK_Pin = D5; 
int SDI_Pin = D7; 

#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH) 

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)

#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)

#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)

#define isEPD_W21_BUSY digitalRead(BUSY_Pin)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);

void driver_delay_us(unsigned int xus)  //1us
{
    os_delay_us(xus);
}
void driver_delay_xms(unsigned long xms) //1ms
{ 
    delay(xms);
}

void SPI_Delay(unsigned char xrate)
{
    driver_delay_us(xrate); 
}

void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;  
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
        SPI_Delay(1);
        if(value & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;   
        value = (value << 1); 
        SPI_Delay(1);
        EPD_W21_CLK_1; 
        SPI_Delay(2);
    }
}

unsigned char SPI_Read()
{
    unsigned char value = 0; 
    for(char i=0; i<8; i++) {
        EPD_W21_CLK_0;
        SPI_Delay(1);

        if (digitalRead(SDI_Pin)) {
            value |= (1 << (7-i)); 
        }
        SPI_Delay(1);
        
        EPD_W21_CLK_1; 
        SPI_Delay(2);
    }
    return value;
}

void EPD_W21_WriteCMD(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

void EPD_W21_WriteDATA(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
     ESP.wdtFeed(); //Feed dog to prevent system reset
  }  
}

void lcd_chkstatus(void)
{
  unsigned char busy;
  do
  {
    //ESP.wdtFeed();
    EPD_W21_WriteCMD(0x71);
    delay(100);
    busy = isEPD_W21_BUSY;
    busy =!(busy & 0x01);
  }
  while(busy);   
  driver_delay_xms(200);                       
}

void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);  
}
void EPD_init(void)
{
    EPD_W21_Init(); 

    Epaper_READBUSY();
    EPD_W21_WriteCMD(0x12);  
    Epaper_READBUSY();

    EPD_W21_WriteCMD(0x3c);         
    EPD_W21_WriteDATA(0x05);    

    EPD_W21_WriteCMD(0x11);  // Data entry mode
    EPD_W21_WriteDATA(0x01);   
    
    EPD_W21_WriteCMD(0x44); 
    EPD_W21_WriteDATA(0x00); // RAM x address start at 0
    EPD_W21_WriteDATA(0x31); // RAM x address end at 31h(49+1)*8->400
    EPD_W21_WriteCMD(0x45); 
    EPD_W21_WriteDATA(0x2B);   // RAM y address start at 12Bh     
    EPD_W21_WriteDATA(0x01);
    EPD_W21_WriteDATA(0x00); // RAM y address end at 00h     
    EPD_W21_WriteDATA(0x00);  
  
    EPD_W21_WriteCMD(0x4E); 
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteCMD(0x4F); 
    EPD_W21_WriteDATA(0x2B);
    EPD_W21_WriteDATA(0x01);
}

void EPD_DeepSleep(void)
{  
 EPD_W21_WriteCMD(0x10); //enter deep sleep
 EPD_W21_WriteDATA(0x01); 
  delay(100);
}

void EPD_Update(void)
{     
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();    
}

void EPD_Update_Fast(void)
{   
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xC7);
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   
}

void EPD_BaseMap( const unsigned char * datas, bool is_red)
{
  unsigned int i;     
  EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<15000;i++)
   {               
     if (is_red) {
        EPD_W21_WriteDATA(0xff);
     } else {
        EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
     }
   }
  EPD_W21_WriteCMD(0x26);   //Write Black and White image to RAM
   for(i=0;i<15000;i++)
   {               
     if (is_red) {
        EPD_W21_WriteDATA(~pgm_read_byte(&datas[i]));
     }  else {
        EPD_W21_WriteDATA(0);
     }
   }  
   EPD_Update(); 
}


#define _VS(x) (x<<6)
#define VSS  _VS(0b00)
#define VSH1 _VS(0b01)
#define VSL  _VS(0b10)
#define VSH2 _VS(0b11)

const unsigned char ssd1683_full_bwr[] = {
//  0: LUTC x 7 
//  RP    A     B     C     D       SRAB    SRCD
    //0x1,	VSL|0x3f,	VSH1|0x3f,	0x0,	0x0,    0x3,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
//  56: LUTR x 7
    0x1,	VSL|0x1f,	0x0,	VSH2|0x3f,	0x0,    0x1,    0xa,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
//  112: LUTW x 7 
    0x1,	VSL|0x2f,	 0,	0,	0,   0x01,    0x00,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
//  168: LUTB x 7
    0x1,	VSH1|0x2f,	0x0,	 0x0, 0x0,  0x01,    0x00,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,
    0x0,	0x0,	0x0,	0x0,	0x0,    0x0,    0x0,

// FR 
    0x04, // 2: 50hz, 3: 75Hz, 4: 100Hz, 5: 125Hz

// XON
    0x0, 0x0,

//  EOPT    VGH     VSH1    VSH2    VSL     VCOM
//  3F      03      04                      2C
//          -20v    15v     3v      -15v
	0x22,	0x17,	0x41,	0x94,	0x32,	0x36	
};

void EPD_Lut(const unsigned char *lut)
{
	EPD_W21_WriteCMD(0x32);
	for(int i=0; i<227; i++) {
		EPD_W21_WriteDATA(lut[i]);
	}

    // gate voltage
	EPD_W21_WriteCMD(0x3F);
	EPD_W21_WriteDATA(*(lut+227));
	
  EPD_W21_WriteCMD(0x03);
	EPD_W21_WriteDATA(*(lut+228));

    // source voltage
    EPD_W21_WriteCMD(0x04);
	EPD_W21_WriteDATA(*(lut+229));	// VSH
	EPD_W21_WriteDATA(*(lut+230));	// VSH2
	EPD_W21_WriteDATA(*(lut+231));	// VSL

    EPD_W21_WriteCMD(0x2C);
    EPD_W21_WriteDATA(*(lut+232));
}

void EPD_HW_Init_Fast2(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(10); //At least 10ms delay  
  
  EPD_W21_WriteCMD(0x12);  //SWRESET
  Epaper_READBUSY();   

  EPD_W21_WriteCMD(0x3C);     
  EPD_W21_WriteDATA(0x05);  

  // builtin temperature sensor
  EPD_W21_WriteCMD(0x18);
  EPD_W21_WriteDATA(0x80);  

  EPD_W21_WriteCMD(0x22); // Load temperature value
  EPD_W21_WriteDATA(0x90);   // 0x91 
  EPD_W21_WriteCMD(0x20); 
  Epaper_READBUSY();   
  
  EPD_W21_WriteCMD(0x11);  // Data entry mode
  EPD_W21_WriteDATA(0x01);   

  // set window 
  EPD_W21_WriteCMD(0x44); 
  EPD_W21_WriteDATA(0x00); // RAM x address start at 0
  EPD_W21_WriteDATA(0x31); // RAM x address end at 31h(49+1)*8->400
  EPD_W21_WriteCMD(0x45); 
  EPD_W21_WriteDATA(0x2B);   // RAM y address start at 12Bh     
  EPD_W21_WriteDATA(0x01);
  EPD_W21_WriteDATA(0x00); // RAM y address end at 00h     
  EPD_W21_WriteDATA(0x00);

  // set counter 
  EPD_W21_WriteCMD(0x4E); 
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F); 
  EPD_W21_WriteDATA(0x2B);
  EPD_W21_WriteDATA(0x01);

  Epaper_READBUSY();   
  EPD_Lut(ssd1683_full_bwr); 
}

void EPD_ShowNum(int x_startA, int y_startA, const unsigned char * datasA, bool is_red)
{
    unsigned int i;  
    unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
    const unsigned int PART_COLUMN = 32;
    const unsigned int PART_LINE = 64;
    
    x_startA=x_startA/8;//Convert to byte
    x_end=x_startA+PART_LINE/8-1; 
    
    y_start1=0;
    y_start2=y_startA-1;
    if(y_startA>=256)
    {
        y_start1=y_start2/256;
        y_start2=y_start2%256;
    }
    y_end1=0;
    y_end2=y_startA+PART_COLUMN-1;
    if(y_end2>=256)
    {
        y_end1=y_end2/256;
        y_end2=y_end2%256;    
    }   

    EPD_W21_RST_0;  // Module reset   
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10); //At least 10ms delay 

#if 0
    EPD_W21_WriteCMD(0x21);  
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteDATA(0x00);
#endif

    EPD_W21_WriteCMD(0x3C); //BorderWavefrom
    EPD_W21_WriteDATA(0x80);  

    EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
    EPD_W21_WriteDATA(x_startA);    // RAM x address start at 00h;
    EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
    EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
    EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
    EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
    EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
    EPD_W21_WriteDATA(y_end1);    

    EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
    EPD_W21_WriteDATA(0x00); 
    EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
    EPD_W21_WriteDATA(y_start2);
    EPD_W21_WriteDATA(y_start1);

    EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
    for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
    {   
        if (is_red) {
          EPD_W21_WriteDATA(0xff); // white
        } else {
          EPD_W21_WriteDATA(pgm_read_byte(&datasA[i]));
        }
    }   

    EPD_W21_WriteCMD(0x26);
    for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
    {                         
        if (is_red) {
          EPD_W21_WriteDATA(~pgm_read_byte(&datasA[i]));
        } else {
          EPD_W21_WriteDATA(0x00);  // white
        }
    }   
}


void setup() {
    Serial.begin(74880);
    Serial.println("hello");

    pinMode(BUSY_Pin, INPUT); 
    pinMode(RES_Pin, OUTPUT);  
    pinMode(DC_Pin, OUTPUT);    
    pinMode(CS_Pin, OUTPUT);    
    pinMode(SCK_Pin, OUTPUT);    
    pinMode(SDI_Pin, OUTPUT);    

    Serial.println("init");
    EPD_init();

    Serial.println("pic");
    EPD_BaseMap(gImage_basemap, false); 

    Serial.println("user id");
    {
        EPD_W21_CS_0;                   
        EPD_W21_DC_0;
        SPI_Write(0x2e);
        pinMode(SDI_Pin, INPUT); 
        EPD_W21_DC_1;
        unsigned char v[10];
        for (char i=0; i<sizeof(v); i++) {
            v[i] = SPI_Read();
        }
        pinMode(SDI_Pin, OUTPUT); 
        EPD_W21_DC_0;
        EPD_W21_DC_1;
        for (char i=0; i<sizeof(v); i++) {
            Serial.printf("%d: %02x\n", i, v[i]);
        }
    }

    Serial.println("OTP reg");
    {
        EPD_W21_CS_0;                   
        EPD_W21_DC_0;
        SPI_Write(0x2d);
        pinMode(SDI_Pin, INPUT); 
        EPD_W21_DC_1;
        unsigned char v[11];
        for (char i=0; i<sizeof(v); i++) {
            v[i] = SPI_Read();
        }
        pinMode(SDI_Pin, OUTPUT); 
        EPD_W21_DC_0;
        EPD_W21_DC_1;
        for (char i=0; i<sizeof(v); i++) {
            Serial.printf("%d: %02x\n", i, v[i]);
        }
    }

    Serial.println("sleep");
    EPD_DeepSleep(); 

#if 0
    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }

    timeClient.begin();
#endif
}

String cmd = "";
uint64 ts = 0;
uint64 base = 0;
void loop() {
    while (Serial.available()) {
        cmd += char(Serial.read()); 
    }

    if (cmd.length() > 0) {
        base = atoi(cmd.c_str());
        Serial.println(cmd); 
        cmd = "";
    }

    uint64 now = millis();
    if ((now - ts) >= (60*1000)) {
        //timeClient.update();
        //Serial.println(timeClient.getFormattedTime());
        
        uint32 tt = (now / 1000) % (24*3600) + base;
        uint32 hh = tt / 3600; 
        uint32 mm = (tt - (hh * 3600)) / 60;
        //uint32 ss = tt % 60;
        //char s1 = ss % 10;
        //char s2 = ss / 10;
        char m1 = mm % 10;
        char m2 = mm / 10;
        char h1 = hh % 10;
        char h2 = hh / 10;
        
        Serial.println(tt);
       
        EPD_HW_Init_Fast2(); 
        EPD_ShowNum(8, 64+(32*0), (unsigned char*)&Num[m1%10], false);
        EPD_ShowNum(8, 64+(32*1), (unsigned char*)&Num[m2%10], false);
        EPD_ShowNum(8, 64+(32*2), (unsigned char*)&gImage_numdot, true);
        EPD_ShowNum(8, 64+(32*3), (unsigned char*)&Num[h1%10], true);
        EPD_ShowNum(8, 64+(32*4), (unsigned char*)&Num[h2%10], true);
        
        EPD_Update_Fast();
        EPD_DeepSleep(); 
       
        ts = now;
    }

    delay(10);
}