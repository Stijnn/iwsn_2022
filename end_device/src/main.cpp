#include <XBee.h>
#include <DHT.h>
#include <DHT_U.h>

XBee xbee = XBee();

DHT_Unified dht(PIN_A0, DHT11);

XBeeAddress64 coordinator_address = XBeeAddress64(0x0013A200, 0x419313D2);
ZBTxStatusResponse tx_status = ZBTxStatusResponse();

typedef union
{
	float value;
	unsigned char buffer[4];
} float_sensor_data;


void setup() {
	Serial.begin(9600);
	xbee.begin(Serial);
	dht.begin();
}

void loop() {
	sensors_event_t temp_event, humidity_event;
	if (dht.temperature().getEvent(&temp_event))
	{
		if (!isnan(temp_event.temperature))
		{
			Serial.println(temp_event.temperature);
		}
	}

	if (dht.humidity().getEvent(&humidity_event))
	{
		if (!isnan(humidity_event.relative_humidity))
		{
			Serial.println(humidity_event.relative_humidity);
		}
	}
  
	char temperature_str[8];
	strcpy(temperature_str, "");
	dtostrf(temp_event.temperature, 3, 2, &temperature_str[strlen(temperature_str)]);

	char humidity_str[8];
	strcpy(humidity_str, "");
	dtostrf(humidity_event.relative_humidity, 3, 2, &humidity_str[strlen(humidity_str)]);

	char msg[17];
	sprintf(msg,"%s %s", temperature_str, humidity_str);
	
	ZBTxRequest dht_sensor_data = ZBTxRequest(coordinator_address, (uint8_t*)msg, 17);
	xbee.send(dht_sensor_data);

	if (xbee.readPacket(500)) {         	
		if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
			xbee.getResponse().getZBTxStatusResponse(tx_status);
		}
	}

	delay(1000);
}