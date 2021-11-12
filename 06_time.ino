void setupTime() {
  timeClient.begin(0);
  setSyncProvider(syncNTPTime);
  setSyncInterval(10);

  #ifdef DEBUGGING
    Serial.println("timezone initialized");
  #endif
}

void updateTime() {
  if (timeStatus() == timeSet && !syncIntervalSet) {
    setSyncInterval(sync_time);
    syncIntervalSet = true;
  }
}

time_t syncNTPTime() {
  unsigned long cur_time, update_time;
  unsigned int drift_time;
  cur_time = timeClient.getEpochTime();
  timeClient.update();
  update_time = timeClient.getEpochTime();
  drift_time = (update_time - cur_time);
  #ifdef DEBUGGING
    Serial.println("NTP Time Sync <=====================================");
    Serial.print("NTP Epoch: "); Serial.println(timeClient.getEpochTime());
    Serial.print("NTP Time : "); Serial.println(timeClient.getFormattedTime());
    Serial.print("Epoch Pre Sync:  "); Serial.println(cur_time);
    Serial.print("Epoch Post Sync: "); Serial.println(update_time);
    Serial.print("Drift Correct:   "); Serial.println(drift_time);
  #endif
  return timeClient.getEpochTime();
}
