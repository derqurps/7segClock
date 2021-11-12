# 7 Segment Clock

Code for the 7segClock as seen here: https://www.instructables.com/Retro-7-Segment-Clock-the-Final-Ones/

## Changed the code for use with a arduino nano 33 IOT

- Removed RTC; syncronizes clock per NTP
- added MQTT
- structured in different files

Webserver does not work (yet?)


uses a arduino_secrets.h file -> see arduino_secrets_demo.h for used variables