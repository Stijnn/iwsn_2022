#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>


EthernetClient eth;
PubSubClient mqtt;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 50);
IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void connect_mqtt()
{
  Serial.println("Attempting Connect");

  if (mqtt.connected())
    return;

  if (mqtt.connect("IWSN_COORDINATOR"))
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

  mqtt.setClient(eth);
  mqtt.setBufferSize(256);
  mqtt.setServer(IPAddress(192, 168, 1, 9), 9001);

  Serial.println(eth.status());
  Serial.print("Coordinator Connected from IP: ");
  Serial.println(Ethernet.localIP());

  connect_mqtt();
}


void loop() {
  delay(1000);
}