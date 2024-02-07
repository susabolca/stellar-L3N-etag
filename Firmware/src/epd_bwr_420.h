#pragma once

uint8_t EPD_BWR_420_detect(void);
uint8_t EPD_BWR_420_read_temp(void);
uint8_t EPD_BWR_420_Display_BWR(unsigned char *image, unsigned char *red_image, int width, int height, int left, int top);
void EPD_BWR_420_sleep(void);
