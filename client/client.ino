#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "BlackCat_tmod";
const char *pass = "BlackCatPass";

// This is client code

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

void setup()
{
  Serial.begin(57600);
  Serial.println("");
  Serial.println("");

  connectToAP();
}

void loop()
{
  // Check WiFi connectivity;
  if (WiFi.status() != WL_CONNECTED)
  {
    // Reconnect
    Serial.println("Disconnected.");
    while (1)
    {
      if (connectToAP())
        break;
    }
  }
  else
  {
    // We're connected, rock on!

    int packetSize = udp.parsePacket();
    if (packetSize){
      int len = udp.read(incomingBuffer, bufferSize);    
      Serial.write(incomingBuffer, packetSize);
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
        udp.beginPacket(ServerIP, udpPort); // remote IP and port
        udp.write(buf2, i2);
        udp.endPacket();
        i2 = 0;
      }    
  }
}

bool connectToAP()
{
  Serial.println("Connecting ...");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass); //Connect to access point
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Start UDP
  Serial.println("Starting UDP");
  udp.begin(udpPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  udp.beginPacket(ServerIP, udpPort); //Send Data to Slave unit
  udp.write("UdpHandshake");
  udp.endPacket();
}
