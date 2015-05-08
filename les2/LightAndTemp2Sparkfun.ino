// App for feeding data from the light sensor to Sparkfun
// Handle returning code and reset ethernet module if needed

#include <EtherCard.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// change these settings to match your own setup
const char publicKey[] PROGMEM = "";
const char privateKey[] PROGMEM = "";
const char website[] PROGMEM = "data.sparkfun.com";

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider


// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x22,0x33,0x44,0x55,0x66,0x77 };
byte Ethernet::buffer[1100];
uint32_t timer;
Stash stash;
byte session;

//timing variable
int res = 1100;

void setup () {
  Serial.begin(57600);
  Serial.println("\n[Data 2 Sparkfun]");

  //Initialize Ethernet
  initialize_ethernet();
  dht.begin();
}


void loop () {

  //if correct answer is not received then re-initialize ethernet module
  if (res > 1400){
    initialize_ethernet();
  }

  res = res + 1;

  ether.packetLoop(ether.packetReceive());

  //1200 res = 60 seconds (50ms each res)
  if (res == 1200) {

    //read light sensor
    photocellReading = analogRead(photocellPin);
    Serial.print("Analog reading = ");
    Serial.println(photocellReading);     // the raw analog reading

    float h = dht.readHumidity();
    float t = dht.readTemperature();


    // by using a separate stash,
    // we can determine the size
    // of the generated message
    // ahead of time
    byte sd = stash.create();
    stash.print("light=");
    stash.print(photocellReading);
    stash.print("&humidity=");
    stash.print(h);
    stash.print("&temp=");
    stash.println(t);
    stash.save();

    //Display data to be sent
    Serial.println("Try to stash request");

    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("POST /input/$F.txt HTTP/1.1" "\r\n"
      "Host: $F" "\r\n"
      "Phant-Private-Key: $F" "\r\n"
      "Connection: close" "\r\n"
      "Content-Type: application/x-www-form-urlencoded" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
     publicKey, website, privateKey, stash.size(), sd);

    Serial.println("Request stashed, try sending");

    // send the packet - this also releases all stash buffers once done
    session = ether.tcpSend();

    Serial.println("Request sent");

  }

   const char* reply = ether.tcpReply(session);

   if (reply != 0) {
     res = 0;
     Serial.println("Incomming response");
     Serial.println(reply);
   }
   delay(50);
}



void initialize_ethernet(void){
  for(;;){ // keep trying until you succeed
    //Reinitialize ethernet module
    pinMode(5, OUTPUT);
    Serial.println("Reseting Ethernet...");
    digitalWrite(5, LOW);
    delay(1000);
    digitalWrite(5, HIGH);
    delay(500);

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0){
      Serial.println( "Failed to access Ethernet controller");
      continue;
    }

    if (!ether.dhcpSetup()){
      Serial.println("DHCP failed");
      continue;
    }

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

    if (!ether.dnsLookup(website))
      Serial.println("DNS failed");

    ether.printIp("SRV: ", ether.hisip);

    //reset init value
    res = 1100;
    break;
  }
}
