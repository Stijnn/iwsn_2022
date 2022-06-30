#include <Arduino.h>
#include <pins_arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <XBee.h>

EthernetClient eth;
PubSubClient mqtt;
XBee xbee = XBee();
ZBRxResponse rx = ZBRxResponse();

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 50);
IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void connect_mqtt()
{
  if (mqtt.connected())
    return;

  if (mqtt.connect("IWSN_COORDINATOR", "coordinator", "secretpassword"))
  {
    mqtt.publish("AUTH", "CONNECTED");
  }
  else
  {
    delay(2000);
    connect_mqtt();
  }
}


void setup() {
    Ethernet.begin(mac, ip, dns, gateway, subnet);

    Serial.begin(9600);
    xbee.begin(Serial);

    mqtt.setClient(eth);
    mqtt.setBufferSize(256);
    mqtt.setServer(IPAddress(192, 168, 1, 19), 9001);

    Serial.println(eth.status());
    Serial.print("Coordinator Connected from IP: ");
    Serial.println(Ethernet.localIP());

    connect_mqtt();
}


void loop() {
    if (!mqtt.connected())
        connect_mqtt();

    if (xbee.readPacket(500))
    {
        if (xbee.getResponse().isAvailable())
        {
            mqtt.publish("DATA", "New Packet");
            if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
            {
                xbee.getResponse().getZBRxResponse(rx);
                mqtt.publish("DATA", "Packet parsed");

                char payload[rx.getDataLength()];
                uint8_t* xbee_buffer = rx.getData();
                memcpy(payload, xbee_buffer, rx.getDataLength());
                mqtt.publish("DATA", payload, rx.getDataLength());
            }
        }
    }

    delay(1000);
}