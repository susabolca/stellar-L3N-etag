#include "epd_ble_service.h"

#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"

#include "epd.h"
#include "ble.h"

extern uint8_t epd_temp[epd_buffer_size];

#define ASSERT_MIN_LEN(val, min_len) \
	if (val < min_len)               \
	{                                \
		return 0;                    \
	}

extern uint8_t epd_buffer[epd_buffer_size];
unsigned int byte_pos = 0;

int epd_ble_handle_write(void *p)
{
	rf_packet_att_write_t *req = (rf_packet_att_write_t *)p;
	uint8_t *payload = &req->value;
	unsigned int payload_len = req->l2capLen - 3;
	uint8_t out_buffer[20] = {0};

	ASSERT_MIN_LEN(payload_len, 1);

	switch (payload[0])
	{
	// Clear EPD display.
	case 0x00:
	    ASSERT_MIN_LEN(payload_len, 2);
		memset(epd_buffer, payload[1], epd_buffer_size);
		memset(epd_temp, payload[1], epd_buffer_size);
		ble_set_connection_speed(40);
		return 0;
	// Push buffer to display.
	case 0x01:
		ble_set_connection_speed(200);
		//EPD_Display(epd_buffer, epd_temp, epd_buffer_size, payload[1]);
		return 0;
	// Set byte_pos.
	case 0x02:
		ASSERT_MIN_LEN(payload_len, 3);
		byte_pos = payload[1] << 8 | payload[2];
		return 0;
	// Write data to image buffer.
	case 0x03:
		if ((payload[2] << 8 | payload[3]) + payload_len - 4 >= epd_buffer_size + 1)
		{
		    out_buffer[0] = 0x00;
		    out_buffer[1] = 0x00;
		    bls_att_pushNotifyData(EPD_BLE_CMD_OUT_DP_H, out_buffer, 2);
			return 0;
		}
		if (payload[1] == 0xff) {
		    memcpy(epd_buffer + (payload[2] << 8 | payload[3]), payload + 4, payload_len - 4);
		} else {
		    memcpy(epd_temp + (payload[2] << 8 | payload[3]), payload + 4, payload_len - 4);
		}

		out_buffer[0] = payload_len >> 8;
		out_buffer[1] = payload_len & 0xff;
		bls_att_pushNotifyData(EPD_BLE_CMD_OUT_DP_H, out_buffer, 2);
		return 0;
	case 0x04: // decode & display a TIFF image
		//epd_display_tiff(epd_buffer, byte_pos);
		return 0;
	default:
		return 0;
	}

	return 0;
}
