#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "BlackCat_tmod";
const char *pass = "BlackCatPass";

// This is Access Point "Server" code

IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
unsigned int udpPort = 8888;

#define packTimeout 5 
#define bufferSize 8192
uint8_t buf2[bufferSize];
uint16_t i2=0;
const byte bufferCharSize = 255;
char incomingBuffer[bufferCharSize];
char outgoingBuffer[bufferSize]; //Where we get the UDP data
uint8_t iofs = 0;

bool newData = false;

void setup()
{
  Serial.begin(4800);
  Serial.println("");
  Serial.println("");
  WiFi.softAP(ssid, pass); //Create Access point

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //Start UDP
  Serial.println("Starting UDP");
  udp.begin(udpPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}

void loop()
{
    int packetSize = udp.parsePacket();
    if (packetSize){
      int len = udp.read(incomingBuffer, bufferSize);
      String ustr = incomingBuffer;
      Serial.write(incomingBuffer, packetSize);
    if(ustr == "UdpHandshake"){
        udp.beginPacket(ClientIP, udpPort); //Send Data to Slave unit
        udp.write("HandshakeRecivied");
        udp.endPacket();
    }
  }
  if(Serial.available()) {
  
      // read the data until pause:
      //Serial.println("sa");
      
      while(1) {
        if(Serial.available()) {
          buf2[i2] = (char)Serial.read(); // read char from UART
          if(i2<bufferSize-1) {
            i2++;
          }
        } else {
          //delayMicroseconds(packTimeoutMicros);
          //Serial.println("dl");
          delay(packTimeout);
          if(!Serial.available()) {
            //Serial.println("bk");
            break;
          }
        }
      }
  
      // now send to WiFi:  
      udp.beginPacket(ClientIP, udpPort); // remote IP and port
      udp.write(buf2, i2);
      udp.endPacket();
      i2 = 0;
  }  
}
