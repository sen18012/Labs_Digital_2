/*************************************************************************************************
  ESP32 Web Server
  Ejemplo de creación de Web server 
  Basándose en los ejemplos de: 
  https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
  https://electropeak.com/learn
**************************************************************************************************/
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <WiFi.h>
#include <WebServer.h>
#include <stdint.h>
#include <stdio.h>
#include "HardwareSerial.h"
//************************************************************************************************
// Variables globales
//************************************************************************************************
// SSID & Password
const char* ssid = "CLARO_AA4686";  // Enter your SSID here
const char* password = "7FdCe2f9f8";  //Enter your Password here

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)


//uint8_t LED1pin = 2;
int parqueos = 0;
bool LED1status = LOW;

//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  Serial2.begin(115200, SERIAL_8N1, 16, 17);


//  pinMode(LED1pin, OUTPUT);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_OnConnect); // Directamente desde e.g. 192.168.0.8
//  server.on("/led1on", handle_led1on);
//  server.on("/led1off", handle_led1off);
  
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {

  server.handleClient();
  
  if (Serial2.available()>0){
    //read the incoming byte:
    parqueos = Serial2.read();
//    Serial.println (parqueos);
    }  

  
  

}
//************************************************************************************************
// Handler de Inicio página
//************************************************************************************************
void handle_OnConnect() {
  //LED1status = LOW;
  Serial.println(parqueos);
  server.send(200, "text/html", SendHTML(parqueos));
}
//************************************************************************************************
// Handler de led1on
//************************************************************************************************
//void handle_led1on() {
//  LED1status = HIGH;
//  Serial.println("GPIO2 Status: ON");
//  server.send(200, "text/html", SendHTML(parqueos));
//}
//************************************************************************************************
// Handler de led1off
//************************************************************************************************
//void handle_led1off() {
//  LED1status = LOW;
//  Serial.println("GPIO2 Status: OFF");
//  server.send(200, "text/html", SendHTML(parqueos));
//}
//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML(uint8_t parqueos) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>PARQUEO-MATIC</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 15px 30px;text-decoration: none;display: inline-block; font-size: 15px;margin: 4px auto 35px;cursor: pointer;border-radius: 2px;}\n";
  ptr += ".button1 {background-color: #4CAF50;}\n"; /* Green */
  ptr += ".button2 {background-color: #DC143C;}\n";/* Red */
//  ptr += ".button1 {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
//  ptr += ".button-on {background-color: #3498db;}\n";
//  ptr += ".button-on:active {background-color: #2980b9;}\n";
//  ptr += ".button-off {background-color: #34495e;}\n";
//  ptr += ".button-off:active {background-color: #2c3e50;}\n";
//  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>PARQUEO - MATIC &#128664</h1>\n";
  ptr += "<h3>Control de Parqueos</h3>\n";

//  if (led1stat)
//  {
//    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
//    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
//    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
//    ptr += "<p>Parqueo 4:</p><a class=\"button button1\">DISPONIBLE</a>\n";
//  }
//  else
//  {
//    ptr += "<p>Parqueo 1:</p><a class=\"button button2\">OCUPADO</a>\n";
//    ptr += "<p>Parqueo 2:</p><a class=\"button button2\">OCUPADO</a>\n";
//    ptr += "<p>Parqueo 3:</p><a class=\"button button2\">OCUPADO</a>\n";
//    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
//  }

//
    if (parqueos == 65)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button1\">DISPONIBLE</a>\n";
  }
  else if (parqueos == 66)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 67)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 68)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 69)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 70)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 71)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 72)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 73)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 74)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 75)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 76)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 77)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 78)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 79)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }
    else if (parqueos == 80)
  {
    ptr += "<p>Parqueo 1:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 2:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 3:</p><a class=\"button button1\">DISPONIBLE</a>\n";
    ptr += "<p>Parqueo 4:</p><a class=\"button button2\">OCUPADO</a>\n";
  }


  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
