#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwr_296.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#define BWR_296_Len 50
uint8_t LUT_bwr_296_part[] = {

0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

BWR_296_Len, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 
0x00, 0x00, 0x00, 

};

#define EPD_BWR_296_test_pattern 0xA5
_attribute_ram_code_ uint8_t EPD_BWR_296_detect(void)
{
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
}

#if 0
_attribute_ram_code_ uint8_t EPD_BWR_296_read_temp(void)
{
    uint8_t epd_temperature = 0 ;

    // SW Reset
    EPD_WriteCmd(0x12);

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
    EPD_WriteData(Y_B0);   //0x0127-->(295+1)=296
	EPD_WriteData(Y_B1);
	EPD_WriteData(0x00);
	EPD_WriteData(0x00);

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

    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);

    return epd_temperature;
}
#endif

#if 0
_attribute_ram_code_ uint8_t EPD_BWR_296_Display(unsigned char *image, int size, uint8_t full_or_partial) {
    uint8_t epd_temperature = 0 ;

    // SW Reset
    EPD_WriteCmd(0x12);

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
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x01);

    // Data entry mode setting
    EPD_WriteCmd(0x11);
    EPD_WriteData(0x01);

    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);
    if (full_or_partial == 1) {
        EPD_WriteData(0x10);
        EPD_WriteData(0x1F);
    } else {
        EPD_WriteData(0x00);
        EPD_WriteData(0x0F);
    }

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(Y_B0);   //0x0127-->(295+1)=296
	EPD_WriteData(Y_B1);
	EPD_WriteData(0x00);
	EPD_WriteData(0x00);

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

    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x00);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(Y_B0);
    EPD_WriteData(Y_B1);

    EPD_LoadImage(image, size, 0x24);

    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x00);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(Y_B0);
    EPD_WriteData(Y_B1);

    EPD_WriteCmd(0x26);
    int i;
    for (i = 0; i < size; i++)
    {
        EPD_WriteData(0x00);
    }

    if (!full_or_partial)
    {
        EPD_WriteCmd(0x32);
        for (i = 0; i < sizeof(LUT_bwr_296_part); i++)
        {
            EPD_WriteData(LUT_bwr_296_part[i]);
        }
    }

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);

    // Master Activation
    EPD_WriteCmd(0x20);

    return epd_temperature;
}
#endif

_attribute_ram_code_ uint8_t EPD_BWR_296_Display_BWR(unsigned char *image, unsigned char *red_image, int width, int height, int left, int top) {
#if 0
    if (red_image == NULL) {
        return EPD_BWR_296_Display(image, size, full_or_partial);
    }
#endif
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

#if 0
    int i;
    if (!full_or_partial)
    {
        EPD_WriteCmd(0x32);
        for (i = 0; i < sizeof(LUT_bwr_296_part); i++)
        {
            EPD_WriteData(LUT_bwr_296_part[i]);
        }
    }
#endif

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);

    // Master Activation
    EPD_WriteCmd(0x20);

    return epd_temperature;
}

_attribute_ram_code_ void EPD_BWR_296_set_sleep(void)
{
    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}

