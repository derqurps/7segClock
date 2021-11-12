void mqttSetup() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqtt_callback);
  mqttReconnect();
  #ifdef DEBUGGING
    Serial.println("mqtt setup finished");
  #endif
}

unsigned long delayStart = 0; // the time the delay started
bool delayRunning = true; // true if still waiting for delay to finish

void mqttReconnect() {
  // Loop until we're reconnected
  if (!mqttClient.connected()) {
    if (delayRunning && ((millis() - delayStart) >= 5000)) {
      delayRunning = false;
      #ifdef DEBUGGING
        Serial.print("Attempting MQTT connection...");
      #endif
      // Attempt to connect
  
      if (mqttClient.connect(mdns_hostname, MQTT_USER, MQTT_PASSWORD, 0, 0, 0, 0, true)) {
        #ifdef DEBUGGING
          Serial.println("connected");
        #endif
        // Subscribe or resubscribe to a topic
        // You can subscribe to more topics (to control more LEDs in this example)
        mqttClient.subscribe(mqtt_listen);
      } else {
        #ifdef DEBUGGING
          Serial.print("failed, rc=");
          Serial.print(mqttClient.state());
          Serial.println(" try again in 5 seconds");
        #endif

        delayStart = millis();   // start delay
        delayRunning = true;
      }
    }
  }
}

void mqtt_callback(char* topic, byte* message, unsigned int length) {

  #ifdef DEBUGGING
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.println();
  #endif
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);
  if(doc["color"]) {
    uint16_t r = doc["color"]["r"].as<int16_t>();
    uint16_t g = doc["color"]["g"].as<int16_t>();
    uint16_t b = doc["color"]["b"].as<int16_t>();
    
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    switchColor(r, g, b);
  } else if(doc["paletteIndex"]) {
    uint8_t pi = doc["paletteIndex"].as<int8_t>();
    switchPalette(pi);
  }
}

void mqttLoopOps() {
  if (!mqttClient.loop()) {
    mqttReconnect();
  }
}

void build_registration() {
  const size_t CAPACITY = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<CAPACITY> doc;
  doc["name"] = mdns_hostname;
  doc["state"] = "connected";
  doc["uptime"] = millis();
  serializeJson(doc, mqttRegistrationStr);
  #ifdef DEBUGGING
    Serial.println("build_registration finished");
  #endif
}

void mqttCheckIn() {
  unsigned long uptime = millis();
  char registration[1024];
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, mqttRegistrationStr);
  doc["uptime"] = millis();
  serializeJson(doc, registration);
  mqttClient.publish(mqtt_registration, registration);
  
}
