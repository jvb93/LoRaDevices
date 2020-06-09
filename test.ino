#include <TinyGPS++.h>
#include <axp20x.h>
#include <WiFi.h>

TinyGPSPlus gps;
HardwareSerial GPS(1);
AXP20X_Class axp;



WiFiServer server(80);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String header;

void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22);
    if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS))
    {
        Serial.println("AXP192 Begin PASS");
    }
    else
    {
        Serial.println("AXP192 Begin FAIL");
    }
    axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
    axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
    axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
    GPS.begin(9600, SERIAL_8N1, 34, 12); //17-TX 18-RX

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    { // If a new client connects,
        smartDelay(500);
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected() && currentTime - previousTime <= timeoutTime)
        { // loop while the client's connected
            currentTime = millis();
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /26/on") >= 0)
                        {
                            Serial.println("GPIO 26 on");
                         
                        }
                        else if (header.indexOf("GET /26/off") >= 0)
                        {
                            Serial.println("GPIO 26 off");
                           
                        }
                        else if (header.indexOf("GET /27/on") >= 0)
                        {
                            Serial.println("GPIO 27 on");
                           
                        }
                        else if (header.indexOf("GET /27/off") >= 0)
                        {
                            Serial.println("GPIO 27 off");
                           
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println("h1 { text-align:center;}");
                        client.println(".button2 {background-color: #555555;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>GPS Data</h1>");
                        client.println("<dl>");
                        client.println("<dt>Time</dt>");
                        client.printf("<dd>%i:%i</dd>", gps.time.hour(), gps.time.minute());
                        client.println("<dt>Lat</dt>");
                        client.printf("<dd>%.4f</dd>", gps.location.lat());
                        client.println("<dt>Long</dt>");
                        client.printf("<dd>%.4f</dd>", gps.location.lng());
                        client.println("<dt>Alt</dt>");
                        client.printf("<dd>%.1f feet</dd>", gps.altitude.feet() / 3.2808);
                        client.println("<dt>Sat</dt>");
                        client.printf("<dd>%i</dd>", gps.satellites.value());
                        client.println("</dl>");
                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
static void smartDelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (GPS.available())
            gps.encode(GPS.read());
    } while (millis() - start < ms);
}