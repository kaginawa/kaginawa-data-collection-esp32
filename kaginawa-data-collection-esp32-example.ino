#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";
const char* url = "http://<HOST>/report";
const String apiKey = "<API_KEY>";
const String hostname = "<HOSTNAME>";
const String customID = "<CUSTOM_ID>";
unsigned long lastTime = 0;
unsigned long timerDelayMin = 3;
unsigned long seq = 0;

String ip2Str(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++) {
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("[Wi-Fi] Connecting to SSID=");
  Serial.print(ssid);
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("[Wi-Fi] Connected: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (lastTime == 0 || (millis() - lastTime) > timerDelayMin * 60 * 1000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[Wi-Fi] Disconnected. Restarting...");
      ESP.restart();
    } else {
      seq++;
      Serial.print("[Client] Sending seq=");
      Serial.println(seq);
      WiFiClient client;
      HTTPClient http;
      http.begin(client, url);
      http.addHeader("Accept", "application/json");
      http.addHeader("Authorization", "token " + apiKey);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{"
          "\"id\":\"" + WiFi.macAddress() + "\","
          "\"trigger\":" + timerDelayMin + ","
          "\"runtime\":\"baremetal xtensa\","
          "\"success\":true,"
          "\"seq\":" + seq + ","
          "\"agent_version\":\"v0.0.0\","
          "\"custom_id\":\"" + customID + "\","
          "\"adapter\":\"eth0\","
          "\"ip4_local\":\"" + ip2Str(WiFi.localIP()) + "\","
          "\"hostname\":\"" + hostname + "\","
          "\"payload_cmd\":\"embedded\","
          "\"payload\":\"{"
          "\\\"sample_value1\\\":123," // Replace here with what you want to send
          "\\\"sample_value2\\\":456"
          "}\"}");
      if (httpResponseCode == 201) {
        Serial.println("[Client] HTTP 201 Created");
      } else {
        Serial.print("[Client] HTTP ERROR: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    lastTime = millis();
  }
}

