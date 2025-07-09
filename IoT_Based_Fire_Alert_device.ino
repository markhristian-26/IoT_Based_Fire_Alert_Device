#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6HYBupw04"
#define BLYNK_TEMPLATE_NAME "Sir cenon project"
#define BLYNK_AUTH_TOKEN "ZvRLq8BFv-tAGabKNpRdA2vQsvKd0l_j"
#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

char auth[] = "ZvRLq8BFv-tAGabKNpRdA2vQsvKd0l_j"; 
char ssid[] = "GA12";
char pass[] = "pcun6912";

const int flame_sensor = 0 ; //Connected to D3 pin of NodeMCU
const int smoke_sensor = A0; //Connected to A0 pin of NodeMCU
int led = 14;
int buzzer = 13;

BlynkTimer timer;

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------
WiFiClientSecure client; //--> Create a WiFiClientSecure object.
String GAS_ID = "AKfycbybRgjIp_lgIxmBKxl6XGgCHBtLkjIeUv5qW_d-PfcrvbTGFGj0OQwbx0zt1NzoFPxohw"; //--> spreadsheet script ID

void setup()
{
  pinMode(flame_sensor, INPUT_PULLUP);
  pinMode(smoke_sensor, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Feraer          ");
  lcd.setCursor(0,1);
  lcd.print("Macapinlac      ");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Project for    ");
  lcd.setCursor(0,1);
  lcd.print("ECEN 100        ");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("IoT Based       ");
  lcd.setCursor(0,1);
  lcd.print("Fire Alarm      ");
  delay(2000);
  lcd.clear();    
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  WiFi.begin("GA12", "pcun6912"); //--> Connect to your WiFi router
  Serial.println("");
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}


void loop() 
{
  int fSV = digitalRead(flame_sensor);
  int sSV = analogRead(smoke_sensor);

  digitalWrite(led, LOW);
  digitalWrite(buzzer, LOW);
  
  Serial.print("Flame Sensor Value: ");
  Serial.println(fSV);
  Serial.print("Smoke Sensor Value: ");
  Serial.println(sSV);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Flame Value: ");
  lcd.println(fSV);
  lcd.setCursor(0, 1);
  lcd.print("Smoke Value: ");
  lcd.println(sSV);

  Blynk.virtualWrite(V0, fSV);
  Blynk.virtualWrite(V2, sSV);
  
 if (fSV == 0)
 { 
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Flame Value: ");
    lcd.print(fSV);
    lcd.setCursor(0,0);
    lcd.print("Fire detected!!");
    delay(2000);
    lcd.clear();
    Blynk.notify("Fire Detected!!");  
  } 

 if (sSV > 350) 
 {
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Smoke Value: ");
    lcd.print(sSV);
    lcd.setCursor(0,0);
    lcd.print("Smoke detected!!");
    delay(2000);
    lcd.clear();
    Blynk.notify("Smoke Detected!!");  
  }  
  delay(1500);
  Blynk.run();
  timer.run();

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Check if any reads failed and exit early (to try again).
  if (isnan(sSV) || isnan(fSV)) {
    Serial.println("Failed to read from the sensors !");
    delay(500);
    return;
  }
  String smo = "Smoke : " + String(sSV) + " ppm";
  String fla = "Flame : " + String(fSV) + " ";
  Serial.println(smo);
  Serial.println(fla);
  
  sendData(fSV, sSV); //--> Calls the sendData Subroutine
}

// Subroutine for sending data to Google Sheets


// Subroutine for sending data to Google Sheets



void sendData(int fla, int smo) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_smoke =  String(smo);
  // String string_temperature =  String(smo, DEC); 
  String string_flame =  String(fla, DEC); 
  String url = "/macros/s/" + GAS_ID + "/exec?Smoke=" + string_smoke + "&Flame=" + string_flame;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 
 
