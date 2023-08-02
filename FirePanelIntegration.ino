/*
 Repeating Web client

 This sketch connects to a web server and makes a request
 using a WIZnet Ethernet shield. You can use the Arduino Ethernet Shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a WIZnet Ethernet module on board.

 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 19 Apr 2012
 by Tom Igoe
 modified 21 Jan 2014
 by Federico Vanzati

 https://www.arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.

 */

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the Ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 10, 177);
IPAddress myDns(192, 168, 10, 99);

// initialize the library instance:
EthernetClient client;

//char server[] = "www.arduino.cc";  // also change the Host line in httpRequest()
IPAddress server(192,168,10,30);

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1000;  // delay between updates, in milliseconds


int digitalInputPin = 2;

void setup() {
  pinMode(digitalInputPin, INPUT_PULLUP);//_PULLUP
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  attachInterrupt(digitalInputPin, handleInterrupt, CHANGE);
  delay(1000);
}

volatile int lastInputState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void loop() {
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

   unsigned long currentMillis = millis();

  // Check if enough time has passed since the last debounce
  if (currentMillis - lastDebounceTime >= debounceDelay) {
    int currentInputState = digitalRead(digitalInputPin);

    // Check for a change in the digital input (0 to 1 or 1 to 0)
    if (currentInputState != lastInputState) {
      // Make the POST request
      // Make the POST request
      if(currentInputState == 1){
        Serial.println("connection 1111");
        const char* jsonData = "{\"status\": false}";
        httpRequest(jsonData);
      }else{
        Serial.println("connection 0000");
        const char* jsonData = "{\"status\": true}";
        httpRequest(jsonData);
      }

      // Update the last input state with the current state
      lastInputState = currentInputState;
    }

    // Update the debounce time
    lastDebounceTime = currentMillis;
  }
  
  if (millis() - lastConnectionTime > postingInterval) {
    int currentInputState = digitalRead(digitalInputPin);
    Serial.println(currentInputState);
    // note the time that the connection was made:
    lastConnectionTime = millis();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest(const char* jsonData) {
  // close any connection before send a new request.
  // This will free the socket on the Ethernet shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // Start the HTTP POST request
    
    //const char* jsonData = "{\"status\": false}";
    client.println("POST /Canar/Api/FR/FireAlarm HTTP/1.1");
    client.println("Host: 192.168.10.30");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); // Specify the length of the JSON data
    client.println(strlen(jsonData));
    client.println();

    // Send the JSON data
    client.println(jsonData);
    client.println();

    
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
