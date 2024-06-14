#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUILDIN_LED 2
#define BUTTON_PIN D3

// Replace with your network credentials
const char* ssid = "RBPI3";
const char* password = "rekashastia";

// Root certificate for howsmyssl.com
const char IRG_Root_X1 [] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIID5TCCAs2gAwIBAgIUA+LaOgK7EM1Kx+c9eO79KI/ZxcQwDQYJKoZIhvcNAQEL
BQAwgYExCzAJBgNVBAYTAlJVMREwDwYDVQQIDAhNb2MIc2NvdzEPMA0GA1UEBwwG
TW9zY293MQ4wDAYDVQQKDAVuaWd1cjEXMBUGA1UEAwwOMTg1LjE4NS42OS4xOTcx
JTAjBgkqhkiG9w0BCQEWFnNpdGxlbW4IYW42OEBnbWFpbC5jb20wHhcNMjQwNDI5
MTg0NjU3WhcNMjUwNDI5MTg0NjU3WjCBgTELMAkGA1UEBhMCUlUxETAPBgNVBAgM
CE1vYwhzY293MQ8wDQYDVQQHDAZNb3Njb3cxDjAMBgNVBAoMBW5pZ3VyMRcwFQYD
VQQDDA4xODUuMTg1LjY5LjE5NzElMCMGCSqGSIb3DQEJARYWc2l0bGVtbghhbjY4
QGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM9SvaP8
Z5OCy5TLSNCGuWJ7JT0h8uhVrQD3Wu2Qv/Ui0NRnIXBkLiYBOjdSy1ma0JiDad9/
SdgOKHUyBmZK8cGiFPKtfBsmpS9pxwMWqQ0AuZjdhxni6fXkbFg2gSZRNDn+GHd/
TuRaOL7AZGIikFE8kpEh9sUCQHQ8sfbUkYaa1PyPnFudfDTna+86G507v8dBYrv1
G+dL6qzIzdwOqI0xFndan1NzlFVD2NlFboBkkGo5EIaEVfU9dcosuQccQxj28QMN
3l3bxHYFYaZAOQWHrrFM1dyCenGN6ZZboVNi8liH6UGSTNk5x+ukOrd23iPiHn1W
hp4pnP9L2oAbS4ECAwEAAaNTMFEwHQYDVR0OBBYEFJMkK2jGvJpVgA6qBpbVaghO
BuTtMB8GA1UdIwQYMBaAFJMkK2jGvJpVgA6qBpbVaghOBuTtMA8GA1UdEwEB/wQF
MAMBAf8wDQYJKoZIhvcNAQELBQADggEBAH3kafnqAGvmiOgpXhbyc1ozbHajSus6
ThlaobkaE/lGSutg8z1pc/ki+GhxuJuFFBpJ+J0vpFpPj8aCsPZarbGXGZ9pYXDB
cB9eVK89ihmX8u5yV0Yspi7UyNctuqy+kezjA6W3gwQKh8dASH5LflqbaBbSVg5S
bk+LuUC+Z9ZOYMgrxIXhU1CQxa5VE3LuI0gY0u7d92QkQNtqrOfPrsN0Do+8Nl8Q
WqQoxiFJFHBRdjxUIah8QkcOjHwSOvR4+6ae7noFry5/7fhPiczZypboVPSh01zX
bii8cBZOhTNaOTuE3qvVVIvF6zivn1mbOHArnypXs/OgGRjCnyvzBh8=
-----END CERTIFICATE-----
)EOF";

// Create a list of certificates with the server certificate
X509List cert(IRG_Root_X1);

// Определяем ширину и высоту дисплея в пикселях
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Определяем объект дисплея
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void printDisplay(String text) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(text);
    display.display();
    Serial.println(text);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(BUILDIN_LED, OUTPUT);
  digitalWrite(BUILDIN_LED, 1);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Инициализация дисплея
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Адрес I2C дисплея может быть 0x3C или 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();

  //Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  printDisplay("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  printDisplay("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  printDisplay("Setup done.");
}

char buffer[100];

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) {
    delay(100);
    return;
  }

  WiFiClientSecure client;

  // wait for WiFi connection
  if ((WiFi.status() != WL_CONNECTED)) {
    printDisplay(String("Unable connect to WIFI"));
    delay(100);
    return;
  }

  client.setTrustAnchors(&cert);
  // client.setInsecure();

  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  digitalWrite(BUILDIN_LED, 0);
  int startTime = millis();
  if (https.begin(client, "https://185.185.69.197:9999/")) {  // HTTPS

    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    int httpCode = https.GET();

    float requestDuration = (millis() - startTime) * 1.0 / 1000;
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        sprintf(buffer, "Key: %s\nWait: %.2f sec.", payload.c_str(), requestDuration);
        printDisplay(String(buffer));
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      sprintf(buffer, "ErrorCode: %s\nWait: %.2f sec.", https.errorToString(httpCode).c_str(), requestDuration);
      printDisplay(String(buffer));
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
    printDisplay(String("Unable connect to server"));
  }
  digitalWrite(BUILDIN_LED, 1);

}