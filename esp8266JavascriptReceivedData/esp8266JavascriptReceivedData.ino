#include <SoftwareSerial.h> 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

EspSoftwareSerial::UART mySerial(D7, D6); // TX = D7, RX = D6
ESP8266WebServer server(80);

String datas;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  WiFi.softAP("Sample_Data");
  Serial.print("Access Point Started@");
  Serial.print("Ip Address: ");
  Serial.println(WiFi.softAPIP());

  // server.on("/", handleRoot);
  // server.on("/data", handleData);
  server.on("/", handleSendData);
  server.begin();
  delay(2000);

}

void loop() {
  if (mySerial.available()) {
    String receivedData = mySerial.readStringUntil('\n'); // Read data
    Serial.print("Received: ");
    Serial.println(receivedData);
    datas = receivedData;
  }else{
    Serial.println("No Data");
  }

  server.handleClient();
  delay(500);
}

void handleSendData(){
  server.send(200, "text/plain", datas);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<title>ESP8266 Data</title></head><body>";
  html += "<h1>Data from Arduino:</h1>";
  html += "<p id='data'>Loading...</p>";
  html += "<script>";
  html += "function fetchData() {";
  html += "  fetch('/data').then(response => response.text()).then(data => {";
  html += "    document.getElementById('data').innerText = data;";
  html += "  });";
  html += "}";
  html += "setInterval(fetchData, 500);"; // Fetch data every 5 seconds
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
void handleData() {
  server.send(200, "text/plain", datas);
}


