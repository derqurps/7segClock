
// Initialize WiFi
void initWiFi() {
  WiFi.begin(ssid, password);
  #ifdef DEBUGGING
    Serial.print("Connecting to WiFi ..");
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  #ifdef DEBUGGING
    Serial.println(WiFi.localIP());
  #endif
}
