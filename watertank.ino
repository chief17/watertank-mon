#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
int watercap = 0;
String floatString="";

ESP8266WebServer server(80);

const int led = 16;

void handleRoot() {
  //digitalWrite(led, 1);
  server.send(200, "text/html", preparehtml());
  //digitalWrite(led, 0);
}
String preparehtml()
{
  String html = "<html>\n  <head>\n  <meta http-equiv=\"Refresh\" content=\"5\">\n   ";
  html+= "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n   <script type=\"text/javascript\">\n      google.charts.load('current', {'packages':['gauge']});\n      google.charts.setOnLoadCallback(drawChart);\n\n      function drawChart() {\n\n        var data = google.visualization.arrayToDataTable([\n          ['Label', 'Value'],\n          ['Water%',"+(String)watercap+"],\n          \n        ]);\n\n        var options = {\n          width: 600, height: 200,\n          redFrom: 0, redTo: 20,\n          yellowFrom:20, yellowTo: 40,\n\u0009\u0009  greenFrom:40, greenTo: 100,\n          minorTicks: 1\n        };\n\n        var chart = new google.visualization.Gauge(document.getElementById('chart_div'));\n\n        chart.draw(data, options);\n\n        \n        \n      }\n    </script>\n  </head>\n  <body>\n    <div id=\"chart_div\" style=\"width: 400px; height: 120px;\"></div>\n\u0009</br>\n\u0009</br>\n\u0009</br>\n\u0009<div><h3>";
  html+= "FLOAT SENSOR STRING(D7-D6-D5-D2-D1): "+floatString+"</h3></div>\n\u0009\n";
  html+="</body>\n</html>";  
  return html;
}
void handleNotFound(){
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}

void setup(void){

  pinMode(D5, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  //pinMode(led, OUTPUT);
  //digitalWrite(led, 1);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

//  server.on("/inline", [](){
//    server.send(200, "text/html", "this works as well");
//  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
long stopfor = 0;
long stopforpost=0;
void loop(void){
  stopfor++;
  stopforpost++;
  if(stopfor > 140000)
  {
    func1();
    stopfor =0;
  }
  if(stopforpost > 14000000)
  {
    post_data();
    stopforpost=0;
  }
    
  server.handleClient();
  
}
void func1 (void) {
  //This executes every 2 seconds.
  Serial.print("FloatString: ");
  Serial.println(floatString);
  watercap=0;
  floatString="";
  if(digitalRead(D7) == 0)
  {
    Serial.print("D7 Tripped ");
    Serial.println(digitalRead(D7)); 
    floatString+="1";
    watercap+=20;
  }
  else
  {
    Serial.println("D7 Open 1"); 
    floatString+="0";
  }
  if(digitalRead(D6) == 0)
  {
    Serial.print("D6 Tripped ");
    Serial.println(digitalRead(D6));
    watercap+=20;  
    floatString+="1";
  }
  else
  {
    Serial.println("D6 Open 1"); 
    floatString+="0";
  }
  if(digitalRead(D5) == 0)
  {
    Serial.print("D5 Tripped ");
    Serial.println(digitalRead(D5));
    watercap+=20; 
    floatString+="1";
  }
  else
  {
    Serial.println("D5 Open 1"); 
    floatString+="0";
  }
  if(digitalRead(D2) == 0)
  {
    Serial.print("D2 Tripped ");
    Serial.println(digitalRead(D2));
    watercap+=20; 
    floatString+="1";
  }
  else
  {
    Serial.println("D2 Open 1"); 
    floatString+="0";
  }
  if(digitalRead(D1) == 0)
  {
    Serial.print("D1 Tripped ");
    Serial.println(digitalRead(D1));
    watercap+=20; 
    floatString+="1"; 
  }
  else
  {
    Serial.println("D1 Open 1"); 
    floatString+="0";
  }
  Serial.println(watercap);
 

  Serial.println("Break");
}

void post_data()
{
    WiFiClient client;
  const String host = "139.59.36.151";
  const int httpPort = 8888;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/?watercap="+(String)watercap;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
}

