#include "Arduino.h"

#ifndef STRUCTS
#define STRUCTS
struct MQTTMessage {
  String topic;
  String payload;
  boolean retain;
};

#endif