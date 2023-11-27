#include <ESP_Mail_Client.h>


#include <dummy.h>

#include <WiFi.h>
#include <WebServer.h>
#define WIFI_SSID "OnePlus 8"
#define WIFI_PASSWORD "12345678"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "drmadhusudhan0@gmail.com"
#define AUTHOR_PASSWORD "jket ioia odvi ddxj"

/* Recipient's email*/
#define RECIPIENT_EMAIL "madhuroyal6363@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);


const char *ssid = "OnePlus 8";
const char *password = "12345678";

const int voltagePin = A0; // Analog pin for voltage sensor
const int currentPin = A1; // Analog pin for current sensor
const int relayPin = 5;    // Digital pin for relay module

float voltage, current, power, energy = 0;

WebServer server(80);

void handleRoot() {
  String html = "<html><head><style>.button{ background-color:black;color:white;font-size:20px;border-radius:7px;}.Power{ font-size:30px;font-family:Times of roman;} .h11{ font-size:40px; font-style:Bold;}</style></head><body><center>";
  html += "<h1 class=\"h11\">Smart Energy Meter</h1>";
  html += "<p class=\"Power\">Power: " + String(power) + " Volt</p>";
  html += "<p class=\"Power\">Energy: " + String(energy) + " Wh</p>";
  html += "<p><a href=\"/off\"><button class=\"button\">Turn On</button></a></p> <p><a href=\"/on\"><button class=\"button\">Turn Off</button></a></p>";
  html += "</center></body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(relayPin, HIGH); // Turn on the bulb
  server.send(200, "text/plain", "OK");
}

void handleOff() {
  digitalWrite(relayPin, LOW); // Turn off the bulb
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Initialize relay off

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  {
Serial.begin(115200);
Serial.println();
Serial.print("Connecting to AP");
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED){
Serial.print(".");
delay(200);
}
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
Serial.println();

/** Enable the debug via Serial port
* none debug or 0
* basic debug or 1
*/
smtp.debug(1);

/* Set the callback function to get the sending results */
smtp.callback(smtpCallback);

/* Declare the session config data */
ESP_Mail_Session session;

/* Set the session config */
session.server.host_name = SMTP_HOST;
session.server.port = SMTP_PORT;
session.login.email = AUTHOR_EMAIL;
session.login.password = AUTHOR_PASSWORD;
session.login.user_domain = "www.gmail.com";

/* Declare the message class */
SMTP_Message message;

/* Set the message headers */
message.sender.name = "ESP";
message.sender.email = AUTHOR_EMAIL;
message.subject = "Smart Energy Meter";
message.addRecipient("", RECIPIENT_EMAIL);

/Send HTML message/
String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Alert! Blub Turned on</h1><p>- Sent from ESP board</p></div>";
message.html.content = htmlMsg.c_str();
message.html.content = htmlMsg.c_str();
message.text.charSet = "us-ascii";
message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

/*
//Send raw text message
String textMsg = "Welcome! To Skill Lync - Sent from ESP board";
message.text.content = textMsg.c_str();
message.text.charSet = "us-ascii";
message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

/* Set the custom message header */
//message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

/* Connect to server with the session config */
if (!smtp.connect(&session))
return;

/* Start sending Email and close the session */
if (!MailClient.sendMail(&smtp, &message))
Serial.println("Error sending Email, " + smtp.errorReason());
}
}

void loop() {
  // Read voltage and current values and calculate power and energy
  voltage = analogRead(voltagePin) * (5.0 / 1023.0);
  current = analogRead(currentPin) * (5.0 / 1023.0);
  power = voltage * current;
  energy += power * 0.001; // Increment energy consumption (assuming loop runs every second)

  server.handleClient();
  delay(1000);
}
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
/* Print the current status */
Serial.println(status.info());

/* Print the sending result */
if (status.success()){
Serial.println("----------------");
ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
Serial.println("----------------\n");
struct tm dt;

for (size_t i = 0; i < smtp.sendingResult.size(); i++){
/* Get the result item */
SMTP_Result result = smtp.sendingResult.getItem(i);
time_t ts = (time_t)result.timestamp;
localtime_r(&ts, &dt);

ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
}
Serial.println("----------------\n");
}
}