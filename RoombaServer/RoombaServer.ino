

#include <SPI.h>
#include <WiFi.h>
#include <SoftwareSerial.h>

char ssid[] = "van_Hilten";      //  your network SSID (name)
char pass[] = "____YOUR_PASSWORD___";   // your network password
int keyIndex = 0;// your network key Index number (needed only for WEP)
bool extra = false;
String extraString = "";
int x ;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
SoftwareSerial Roomba(2, 3); //RX, TX, #2 can handle interrupts on uno.
void setup() {
   //Serial.begin(115200);
  Roomba.begin(112500);      // initialize serial communication
  Roomba.write(128);       //Start the IO interface

  Roomba.write(131);       //Safe mode.


  pinMode(13, OUTPUT);      // set the LED pin mode
  digitalWrite(13, LOW);
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
     //Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 1 second for connection:
    delay(1000);
  }
  server.begin();                           // start the web server on port 80
  
}


void loop() {

//Serial.println("yay");
  digitalWrite(13, HIGH);
  WiFiClient client = server.available();   // listen for incoming clients
  //Serial.print(Serial.read());
  if (client) {                             // if you get a client,
    //Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            ///client.print("Click <a href=\"/DOCK\">here</a> to dock<br>");
            //client.print("Click <a href=\"/CLEAN\">here</a> to clean<br>");
            
            if (extra) {
              client.print(extraString);
              extra = false;
            }
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        
        if (currentLine.endsWith("GET /DOCK")) { //When requesting http://IP/DOCK
          Roomba.write(128);                    //Start roomba IO

          Roomba.write(131);                    //Go to safe mode
          Roomba.write(143); // Seek dock
          

        }
        if (currentLine.endsWith("GET /CLEAN")) {     //http://IP/CLEAN
          Roomba.write(128);

          Roomba.write(131);
          Roomba.write(135);     //G clean
        }
        if (currentLine.endsWith("GET /UP")) {     //To check if it is the correct IP.
          extra = true;
          extraString = "y";
        }
       
      }
    }
    // close the connection:
    client.stop();
    
  }
}


