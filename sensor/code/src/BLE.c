
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif

/*
 * Set iBeacon demo advertisement data. These values are for
 * demonstration only and must be changed for production environments!
 *
 * UUID:  18ee1516-016b-4bec-ad96-bcb96d166e97
 * Major: 0
 * Minor: 0
 * RSSI:  -56 dBm
 */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
		      0x4c, 0x00, /* Apple */
		      0x02, 0x15, /* iBeacon */
		      0x18, 0xee, 0x15, 0x16, /* UUID[15..12] */
		      0x01, 0x6b, /* UUID[11..10] */
		      0x4b, 0xec, /* UUID[9..8] */
		      0xad, 0x96, /* UUID[7..6] */
		      0xbc, 0xb9, 0x6d, 0x16, 0x6e, 0x97, /* UUID[5..0] */
		      0x00, 0x00, /* Major */
		      0x00, 0x00, /* Minor */
		      IBEACON_RSSI) /* Calibrated RSSI @ 1m */
};

// Initialises the bluetooth
static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad),
			      NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("iBeacon started\n");
}

// Callback function when it finds a bluetooth signal
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	
	char addr_str[BT_ADDR_LE_STR_LEN];
    char value[7];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    //char* address_only = strtok(addr_str, " ");

    char string[20] = {ad->data[13],ad->data[14], ad->data[15],ad->data[16],ad->data[17],ad->data[18],ad->data[19]};

    uint8_t data_index = 23;
    value[0] = ad->data[data_index];
    value[1] = ad->data[data_index + 1];
    value[2] = ad->data[data_index + 2];
    value[3] = ad->data[data_index + 3];
    value[4] = ad->data[data_index + 4];
    value[5] = ad->data[data_index + 5];
    value[6] = '\0';
    
	// If the board 1 receives from 2, 
    if (strcmp(string, "THINGY1") == 0&& value[1] == '1') {
    //	k_msgq_put(&msgq, &value, K_NO_WAIT);
	} else if (strcmp(string, "THINGY2") == 0 && value[1] == '2') {
    //	k_msgq_put(&msgq, &value, K_NO_WAIT);
    }
}

// Initialises the bluetooth.
void init_BLE(void) {
	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_PASSIVE,
        .options    = BT_LE_SCAN_OPT_NONE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL_MIN,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};
	/* Initialize the Bluetooth Subsystem */
	bt_enable(bt_ready);

	printk("Bluetooth initialized\n");
	bt_le_scan_start(&scan_param, device_found);

	/* Start advertising */
	bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);


	//bt_le_adv_stop(); // Not used.

}

void send_humidity(void) {

}

void send_temperature(void) {

}

void send_pressure(void) {

}

void send_UV(void) {

}

void send_soil(void) {
    
}







