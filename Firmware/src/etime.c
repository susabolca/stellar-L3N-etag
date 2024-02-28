#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "drivers/8258/flash.h"
#include "i2c.h"
#include "etime.h"
#include "main.h"

RAM uint16_t time_trime = 0; //5000;// The higher the number the slower the time runs!, -32,768 to 32,767 
RAM uint32_t one_second_trimmed = CLOCK_16M_SYS_TIMER_CLK_1S;
RAM uint32_t current_unix_time;
RAM struct date_time current_date = {0};

RAM uint32_t last_clock_increase;
RAM uint32_t last_reached_period[10] = {0};
RAM uint8_t has_ever_reached[10] = {0};

uint8_t map[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

_attribute_ram_code_ void init_time(void)
{
    one_second_trimmed += time_trime;
    current_unix_time = 0;
}

static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

// set rtc
void rtc_adjust(struct date_time *cur) {
#if 0 // ds3231
  	uint8_t buffer[] = {
						bin2bcd(cur->tm_sec),
						bin2bcd(cur->tm_min),
						bin2bcd(cur->tm_hour),
						bin2bcd(cur->tm_week),
						bin2bcd(cur->tm_day),
						bin2bcd(cur->tm_month),
						bin2bcd(cur->tm_year - 2000U)};
	i2c_write_series(0x00, 1, buffer, sizeof(buffer));

    uint8_t statreg = i2c_read_byte(0x0F, 1);
    statreg &= ~0x80; // flip OSF bit
    i2c_write_byte(0x0F, 1, statreg);
#else  
    // PCF8563
  	uint8_t buffer[] = {
						bin2bcd(cur->tm_sec),
						bin2bcd(cur->tm_min),
						bin2bcd(cur->tm_hour),
						bin2bcd(cur->tm_day),
						cur->tm_week,
						bin2bcd(cur->tm_month),
						bin2bcd(cur->tm_year - 2000U)};
	i2c_write_series(0x02, 1, buffer, sizeof(buffer));

    //uint8_t statreg = i2c_read_byte(0x0F, 1);
    //statreg &= ~0x80; // flip OSF bit
    //i2c_write_byte(0x0F, 1, statreg);
#endif
}

// read rtc
void rtc_now(struct date_time *cur)
{
#if 0
    // DS3231
	unsigned char buf[7];
	i2c_read_series(0x00, 1, buf, 7);
    cur->tm_sec = bcd2bin(buf[0] & 0x7f);
    cur->tm_min = bcd2bin(buf[1]);
    cur->tm_hour = bcd2bin(buf[2]);
    cur->tm_week = bcd2bin(buf[3]);
    cur->tm_day = bcd2bin(buf[4]);
    cur->tm_month = bcd2bin(buf[5] & 0x7f);
    cur->tm_year = bcd2bin(buf[6]) + 2000U;
#else
	unsigned char buf[7];
	i2c_read_series(0x02, 1, buf, 7);
    cur->tm_sec = bcd2bin(buf[0] & 0x7f);
    cur->tm_min = bcd2bin(buf[1] & 0x7f);
    cur->tm_hour = bcd2bin(buf[2] & 0x3f);
    cur->tm_day = bcd2bin(buf[3] & 0x3f);
    cur->tm_week = buf[4] & 0x07;
    cur->tm_month = bcd2bin(buf[5] & 0x1f);
    cur->tm_year = bcd2bin(buf[6]) + 2000U;
#endif    
}

_attribute_ram_code_ void handler_time(void)
{
    if (clock_time() - last_clock_increase >= one_second_trimmed)
    {
        last_clock_increase += one_second_trimmed;
        current_unix_time++;
        //printf("%u\n", clock_time());

#if 1
        rtc_now(&current_date);
#else
        current_date.tm_min = (current_unix_time / 60) % 60;
        current_date.tm_hour = ((current_unix_time / 60) / 60) % 24;
        current_date.tm_sec = current_unix_time % 60;

        if (current_unix_time % 86400 == 0) {
            current_date.tm_month = current_date.tm_month % 12;
            if (current_date.tm_day + 1 > map[current_date.tm_month - 1]) {
                current_date.tm_day = 1;
                if (current_date.tm_month + 1 > 12) {
                    current_date.tm_month = 1;
                    current_date.tm_year += 1;
                } else {
                    current_date.tm_month += 1;
                }
            } else {
                current_date.tm_day = current_date.tm_day + 1;
            }

            current_date.tm_week = (current_date.tm_week + 1) % 7;
        }
#endif
    }
}

_attribute_ram_code_ uint8_t time_reached_period(timer_channel ch, uint32_t seconds)
{
    if (!has_ever_reached[ch])
    {
        has_ever_reached[ch] = 1;
        return 1;
    }
    if (current_unix_time - last_reached_period[ch] >= seconds)
    {
        last_reached_period[ch] = current_unix_time;
        return 1;
    }
    return 0;
}

_attribute_ram_code_ void set_time(uint32_t time_now, uint16_t time_year, uint8_t time_month, uint8_t time_day, uint8_t time_week)
{
    current_unix_time = time_now;

    current_date.tm_min = (current_unix_time / 60) % 60;
    current_date.tm_hour = ((current_unix_time / 60) / 60) % 24;
    current_date.tm_sec = current_unix_time % 60;
    
    current_date.tm_year = time_year;
    current_date.tm_month = time_month;
    current_date.tm_day = time_day;
    current_date.tm_week = time_week;

    // adjust rtc
    rtc_adjust(&current_date);
}

_attribute_ram_code_ struct date_time get_time(void) {
    return current_date;
}
