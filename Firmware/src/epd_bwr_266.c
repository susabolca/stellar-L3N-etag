#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwr_266.h"
#include "drivers.h"
#include "stack/ble/ble.h"

// UC8151C or similar EPD Controller
_attribute_ram_code_ uint8_t EPD_BWR_266_read_temp(void)
{
    uint8_t epd_temperature = 0 ;
    EPD_WriteCmd(0x04);

    // check BUSY pin
    EPD_CheckStatus_inverted(100);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    // power off
    EPD_WriteCmd(0x02);

    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);

    return epd_temperature;
}

_attribute_ram_code_ uint8_t EPD_BWR_266_Display_BWR(unsigned char *image, unsigned char *red_image, int width, int height, int left, int top)
{
    uint8_t epd_temperature = 0 ;

    // data size in bytes
    int size = width*height/8;

    EPD_WriteCmd(0x00);
    // 160x296 | UD:0 SHL: 0
    EPD_WriteData((0b11 << 6) | (0b00 << 4) | (0b00 << 2) | 0b11);

#if 0 
    // Booster soft start
    EPD_WriteCmd(0x06);
    EPD_WriteData(0x17);
    EPD_WriteData(0x17);
    EPD_WriteData(0x17);
#endif


#if 0
    // left up corner
    int w0 = left;
    int h0 = top;
    // right bottom corner
    int w1 = w0 + width - 1;
    int h1 = h0 + height/8 - 1;
#endif

    // power on
    EPD_WriteCmd(0x04);
    // check BUSY pin
    EPD_CheckStatus_inverted(100);

#if 0
    EPD_WriteCmd(0x00);
    EPD_WriteData(0x0f);
    EPD_WriteData(0x89);
#endif 

    EPD_WriteCmd(0xe5);
    EPD_WriteData(25);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

#if 0
    // panel setting
    EPD_WriteCmd(0x00);
    if (full_or_partial)
        EPD_WriteData(0b00011111);
    else
        EPD_WriteData(0b00111111);
    EPD_WriteData(0x0f);
#endif

    // resolution setting
    EPD_WriteCmd(0x61);
    EPD_WriteData(height & 0xff);
    EPD_WriteData((width >> 8) & 0b1);
    EPD_WriteData((width & 0xff));

    // Vcom and data interval setting
    EPD_WriteCmd(0X50);
    EPD_WriteData(0x77);

#if 0
    if (!full_or_partial)
    {
        EPD_send_lut(lut_bw_213_20_part, sizeof(lut_bw_213_20_part));
        EPD_send_empty_lut(0x21, 260);
        EPD_send_lut(lut_bw_213_22_part, sizeof(lut_bw_213_22_part));
        EPD_send_lut(lut_bw_213_23_part, sizeof(lut_bw_213_23_part));
        EPD_send_empty_lut(0x24, 260);

        EPD_WriteCmd(0x10);
        int i;
        for (i = 0; i < size; i++)
        {
            EPD_WriteData(~image[i]);
        }
    }
#endif

#if 0
    // Gate/Source start setting
    EPD_WriteCmd(0x65);
    EPD_WriteCmd(0x00);
    EPD_WriteCmd(0x00);
    EPD_WriteCmd(0x00);
#endif

    if (image) {
        EPD_LoadImage(image, size, 0x10);
    } else {
        EPD_WriteCmd(0x10);
        for (int i = 0; i < size; i++) {
            EPD_WriteData(0xff);
        }
    }

    if (red_image) {
        EPD_LoadImage(red_image, size, 0x13);
    } else {
        EPD_WriteCmd(0x13);
        for (int i = 0; i < size; i++) {
            EPD_WriteData(0xff);
        }
    }

    // trigger display refresh
    EPD_WriteCmd(0x12);

    return epd_temperature;
}

_attribute_ram_code_ void EPD_BWR_266_sleep(void)
{
    // Vcom and data interval setting
    EPD_WriteCmd(0x50);
    EPD_WriteData(0xf7);

    // power off
    EPD_WriteCmd(0x02);

    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);
}