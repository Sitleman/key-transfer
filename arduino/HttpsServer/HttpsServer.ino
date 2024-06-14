#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#define GREEN D0 
#define YELLOW D6
#define RED D8
#define GREEN_BRIGHT 150
#define YELLOW_BRIGHT 255
#define RED_BRIGHT 255

int lights[] = {GREEN, YELLOW, RED};
int lights_bright[] = {150, 255, 255};
int lights_size = 3;

const char* ssid = "RBPI3";
const char* password = "rekashastia";
ESP8266WebServerSecure server(8080);

// The certificate is stored in PMEM
static const uint8_t x509[] PROGMEM = R"EOF(
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

// And so is the key.  These could also be in DRAM
static const uint8_t rsakey[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDPUr2j/GeTgsuU
y0jQhrlieyU9IfLoVa0A91rtkL/1ItDUZyFwZC4mATo3UstZmtCYg2nff0nYDih1
MgZmSvHBohTyrXwbJqUvaccDFqkNALmY3YcZ4un15GxYNoEmUTQ5/hh3f07kWji+
wGRiIpBRPJKRIfbFAkB0PLH21JGGmtT8j5xbnXw052vvOhudO7/HQWK79RvnS+qs
yM3cDqiNMRZ3Wp9Tc5RVQ9jZRW6AZJBqORCGhFX1PXXKLLkHHEMY9vEDDd5d28R2
BWGmQDkFh66xTNXcgnpxjemWW6FTYvJYh+lBkkzZOcfrpDq3dt4j4h59VoaeKZz/
S9qAG0uBAgMBAAECggEBAKQRvrI/1VgPcjzWg1FCzQL6oYW3kBv9cWQKXJMUslS0
sL7mvrfLEBpTblbJ1tOaiOmpVefhO/cXfT0B0b6yIJA0TI+ga06xTpgvBaeUD0ST
Bk3LQldiE/TcSEGnIJ3KNaKPS52FylE8dRu/9bET2VHwtazVTAyss4VJYy0LG/+A
q4byAQbT6W52TYbeS+l0xHE3YHkL8wDJJ3iSbRZ2ZPKFHYu1YTOFWQ/ehEh8Vkjq
J13oP4n64S7rcVpikwIyPJItZvA6Jg8gWd3A1gUW7WefpQetX7KtP2WcbSqF1CNh
00Llu0JnQKWfwk1beII29ce4DStbG8YNkY3+U01RfeECgYEA8FCUnldUGrg3cHnQ
rLe2DgLcO7WrSpFFrSWNmTJeaVfm9tqa6J4Ef6OD5rXTRPdBNjAipnf0YawNK09T
LRb0Vlz/7DETCSqZOpe+I1HPc7J3fH0mwbkuiqa72vmwcZ/aQHoaOteR0QU5uMKB
rF4eYYjIuBgVCmzLrD2S0YTzkbMCgYEA3NrnfEFjt8u+Vg7NreGH1k2s+PrGijwF
VLL5eE0clbi8C7TScjt2hXxJBv40r4gb94HCjfrYjo5jWPFzuGC/4VcGLTphZC35
9HSYaXX7FaZltDOBTHyqIOauHADp5lOq28gc5e7x+gHiFplAibNrjzYPduJ4dOHA
s5yk7en0S/sCgYEAuNxj1wI6kmtPSliKq7KOe2y72brAT5Y80RroOAbFFagm5ZH4
RCT6NO2mE52fLZg+M98HaKMx/MiLQNu+khrsTlswC5Mcn5qDUM1V2HmgadHJlJv5
amB3avUj4SjeECn1jE/hSY52fyHT60rE0cF9AW5laSgtV12lt5OOE1qAu30CgYAf
gyB6az4WMH0E/HG+eKqQGl8u22FOwgeksTDFSINmnYg/R1qrr+5WV4h39feAgDQ1
WY9wALW7iXeFblYrnapjNa6M74KL7791cNl6WroruaVTU/ID/tmrfh6jokG9enyr
hV8SM8jU+kXPIajCvRYAxh7owKGdU8TSG+ywgaw41wKBgE8bKlFDTfDvnia/Ti2F
3CYN37dThNG4bBT3tfHUWkrBW8/03B1BP4fozRPgm4pk4hc5jLzz+53gX8CEi080
csM1MLfm0KjRgfn+1iZRbt6R6AGGVHjDq9oCFdNCrAjk4znHiuE0NQ978Ro5PhEC
yOTKFaREDmdTXe3OirCBeGjD
-----END PRIVATE KEY-----
)EOF";

HTTPClient http;

void handleRoot() {
  analogWrite(RED, RED_BRIGHT);
  http.begin("http://185.185.69.197:9998/api/v1/terminal-master-key/terminal_1");
  int httpResponseCode = http.GET();

  // Проверка кода ответа
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
    server.send(200, "text/plain", response);
  } else {
    server.send(500, "text/plain", "Internal error");
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  analogWrite(RED, 0);
}

void setup(void) {
    for (int i = 0; i < lights_size; i++) {
    pinMode(lights[i], OUTPUT);
  }
  Serial.begin(115200);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  analogWrite(YELLOW, YELLOW_BRIGHT);

  Serial.print("Waiting for NTP time sync: ");
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
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
  analogWrite(GREEN, GREEN_BRIGHT);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTPS server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
