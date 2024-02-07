#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwr_420.h"
#include "drivers.h"
#include "stack/ble/ble.h"

// https://www.mdpi.com/2072-666X/12/5/578
#define _VS(x) (x<<6)
#define VSS  _VS(0b00)
#define VSH1 _VS(0b01)
#define VSL  _VS(0b10)
#define VSH2 _VS(0b11)

static const unsigned char ssd1683_full_bwr[] = {
//  0: LUTC x 7 
//  RP      A           B           C           D           SRAB    SRCD
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
//  56: LUTR x 7
    0x1,    VSL|0x1f,   0x0,        VSH2|0x3f,  0x0,        0x1,    0xa,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
//  112: LUTW x 7 
    0x1,    VSL|0x2f,   0x0,        0x0,        0x0,        0x1,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
//  168: LUTB x 7
    0x1,    VSH1|0x2f,  0x0,        0x0,        0x0,        0x1,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,
    0x0,    0x0,        0x0,        0x0,        0x0,        0x0,    0x0,

// FR 
    0x04, // 2: 50hz, 3: 75Hz, 4: 100Hz, 5: 125Hz

// XON
    0x0, 0x0,

//  EOPT    VGH     VSH1    VSH2    VSL     VCOM
//  3F      03      04                      2C
//  22      -20v    15v     3v      -15v
    0x22,   0x17,   0x41,   0x94,   0x32,   0x36    
};


_attribute_ram_code_ uint8_t EPD_BWR_420_detect(void)
{
#if 0
    // SW Reset
    EPD_WriteCmd(0x12);
    WaitMs(10);

    EPD_WriteCmd(0x32);
    int i;
    for (i = 0; i < 153; i++)  // FIXME  DETECT MODEL 296
    {
        EPD_WriteData(EPD_BWR_296_test_pattern);
    }
    EPD_WriteCmd(0x33);
    for (i = 0; i < 153; i++)
    {
        if(EPD_SPI_read() != EPD_BWR_296_test_pattern)
            return 0;
    }
    return 1;
#else
    return 1;
#endif
}

_attribute_ram_code_ uint8_t EPD_BWR_420_read_temp(void)
{
    uint8_t epd_temperature = 0 ;
    
    // SW Reset
    EPD_WriteCmd(0x12);

    EPD_CheckStatus_inverted(100);

#if 0
    // Set Analog Block control
    EPD_WriteCmd(0x74);
    EPD_WriteData(0x54);

    // Set Digital Block control
    EPD_WriteCmd(0x7E);
    EPD_WriteData(0x3B);

    // Booster soft start
    EPD_WriteCmd(0x0C);
    EPD_WriteData(0x8B);
    EPD_WriteData(0x9C);
    EPD_WriteData(0x96);
    EPD_WriteData(0x0F);

    // Driver output control
    EPD_WriteCmd(0x01);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x01);

    // Data entry mode setting
    EPD_WriteCmd(0x11);
    EPD_WriteData(0x01);

    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x2E);
    EPD_WriteData(0x00);
#endif
    
    // Border waveform control
    EPD_WriteCmd(0x3C);
    EPD_WriteData(0x05);

#if 0
    // Display update control
    EPD_WriteCmd(0x21);
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);
#endif

    // Temperature sensor control
    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xB1);    // without display
    
    // Master Activation
    EPD_WriteCmd(0x20);

    EPD_CheckStatus_inverted(100);

    // Temperature sensor read from register
    EPD_WriteCmd(0x1B);
    epd_temperature = EPD_SPI_read();    
    EPD_SPI_read();

    WaitMs(5);
    
    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);

    return epd_temperature;
}

static void EPD_Lut(const unsigned char *lut)
{
    EPD_WriteCmd(0x32);
    for(int i=0; i<227; i++) {
        EPD_WriteData(lut[i]);
    }

    // gate voltage
    EPD_WriteCmd(0x3F);
    EPD_WriteData(*(lut+227));

    EPD_WriteCmd(0x03);
    EPD_WriteData(*(lut+228));

    // source voltage
    EPD_WriteCmd(0x04);
    EPD_WriteData(*(lut+229));    // VSH
    EPD_WriteData(*(lut+230));    // VSH2
    EPD_WriteData(*(lut+231));    // VSL

    EPD_WriteCmd(0x2C);
    EPD_WriteData(*(lut+232));
}

_attribute_ram_code_ uint8_t EPD_BWR_420_Display_BWR(unsigned char *image, unsigned char *red_image, int width, int height, int left, int top) {
    uint8_t epd_temperature = 0 ;

    // SW Reset
    EPD_WriteCmd(0x12);

    // data size in bytes
    int size = width*height/8;
    // left up corner
    int w0 = left;
    int h0 = top;
    // right bottom corner
    int w1 = w0 + width - 1;
    int h1 = h0 + height/8 - 1;

    EPD_CheckStatus_inverted(100);

    // Set Analog Block control
    EPD_WriteCmd(0x74);
    EPD_WriteData(0x54);

    // Set Digital Block control
    EPD_WriteCmd(0x7E);
    EPD_WriteData(0x3B);

    // Booster soft start
    EPD_WriteCmd(0x0C);
    EPD_WriteData(0x8B);
    EPD_WriteData(0x9C);
    EPD_WriteData(0x96);
    EPD_WriteData(0x0F);

    // Driver output control
    EPD_WriteCmd(0x01);
    EPD_WriteData(0x28);  // mux=0x128(296)
    EPD_WriteData(0x01);
    EPD_WriteData(0x01);  // gd=0, sm=0, tb=1

    // Data entry mode setting
    /*(00,00h)         o: begin
        00:  <----     X: decrement
               /       Y: decrement
             <---o

        01:  ---->     X: increment
               \       Y: decrement
             o--->

        10:  <---o     X: decrement
               \       Y: increment
             <----

        11:  o---->     X: increment
                /       Y: increment
             ----->
                    (15,127h)
    */
    EPD_WriteCmd(0x11);
    EPD_WriteData(0x07);    // am=0,id=11

    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);

#if 0
    if (full_or_partial == 1) {
        EPD_WriteData(0x0C);
        EPD_WriteData(0x17);
    } else if (full_or_partial == 2) {
        EPD_WriteData(0x18);
        EPD_WriteData(0x23);
    } else if (full_or_partial == 3) {
        EPD_WriteData(0x24);
        EPD_WriteData(0x2F);
    } else {
        EPD_WriteData(0x00);
        EPD_WriteData(0x0B);
    }
#else
    EPD_WriteData(h0 & 0xff); 
    EPD_WriteData(h1 & 0xff); 
#endif

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(w0 & 0xff);
    EPD_WriteData((w0>>8) & 0xff);
    EPD_WriteData(w1 & 0xff);
    EPD_WriteData((w1>>8) & 0xff);

    // Border waveform control
    EPD_WriteCmd(0x3C);
    EPD_WriteData(0x05);

    // Display update control
    EPD_WriteCmd(0x21);
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);

    // Temperature sensor control
    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xB1);

    // Master Activation
    EPD_WriteCmd(0x20);

    EPD_CheckStatus_inverted(100);

    // Temperature sensor read from register
    EPD_WriteCmd(0x1B);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    WaitMs(5);

    EPD_Lut(ssd1683_full_bwr);

    // black color
    // Set RAM X counter 
    EPD_WriteCmd(0x4E);
    EPD_WriteData(h0 & 0xff);

    // Set RAM Y counter 
    EPD_WriteCmd(0x4F);
    EPD_WriteData(w0 & 0xff);
    EPD_WriteData((w0 >> 8) & 0xff);

    if (image) {
        EPD_LoadImage(image, size, 0x24);
    } else {
        EPD_WriteCmd(0x24);
        for (int i = 0; i < size; i++) {
            EPD_WriteData(0xff);
        }
    }

    // read color
    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(h0 & 0xff);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(w0 & 0xff);
    EPD_WriteData((w0 >> 8) & 0xff);

    if (red_image) {
        EPD_LoadImage(red_image, size, 0x26);
    } else {
        EPD_WriteCmd(0x26);
        for (int i = 0; i < size; i++) {
            EPD_WriteData(0x00);
        }
    }

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);

    // Master Activation
    EPD_WriteCmd(0x20);

    return epd_temperature;
}

_attribute_ram_code_ void EPD_BWR_420_sleep(void)
{
    // deep sleep
    EPD_WriteCmd(0x10);
    // 01: mode 1, 11: mode 2
    EPD_WriteData(0x01);
}

