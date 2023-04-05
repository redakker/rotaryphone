#ifndef ROTARY
#define ROTARY

#include <cstddef>
#include "Callback.h"
#include "definitions.h"
#include "log.cpp"
#include "mqtt.cpp"
#include <LinkedList.h>
#include "RotaryDialer.h"

class Rotary {

    Log* rlog;
    Signal<MQTTMessage>* message;
    String log_prefix = "[ROTARY] ";
 
    public:
        Rotary(Log &rlog) {
            this -> rlog = &rlog;
        }

        void setup (Signal<MQTTMessage> &message) {

            this -> message = &message;
            this -> rlog -> log(log_prefix, "Rotary is ready");
        }

        void loop() {

        }
  

        void receiveCommand(String message) {

            StaticJsonDocument<1000> tempJson; 
            DeserializationError Derror = deserializeJson(tempJson, message);

            this -> rlog -> log(log_prefix, "Message reseived: " + message);

            if (Derror) {
                rlog -> log(log_prefix + (String) "DeserializationError: " + Derror.c_str() + " (receiveCommand) " + message);
                
            } else {

            }
           
        }

    private:

    bool is_number(const String& s)
    {
        return !s.isEmpty() && std::find_if(s.begin(), 
            s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }
     
};

#endif