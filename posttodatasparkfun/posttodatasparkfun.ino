#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Phant arduino lib
// git clone https://github.com/sparkfun/phant-arduino ~/sketchbook/libraries/Phant
#include <Phant.h>

// Arduino example stream
// http://data.sparkfun.com/streams/VGb2Y1jD4VIxjX3x196z
// hostname, public key, private key
Phant phant("data.sparkfun.com", "VGb2Y1jD4VIxjX3x196z", "9YBaDk6yeMtNErDNq4YM");

#include <EtherCard.h>

byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[700];
Stash stash;
const char website[] PROGMEM = "google.com";

void setup() {
  Serial.begin(9600);
  dht.begin();

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));

  Serial.println("Setup completed");

}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  phant.add("humidity", h);
  phant.add("temperature", t);

  Serial.println("Sending data...");
  byte sd = stash.create();
  stash.println(phant.post());
  stash.save();
  ether.tcpSend();

  // Wait a few seconds between measurements.
  delay(10000);
}
