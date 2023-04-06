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
    RotaryDialer dialer = RotaryDialer(PIN_READY, PIN_PULSE);

    String numberToSend = "";
    unsigned long lastGetNumberMillis = 0;
 
    public:
        Rotary(Log &rlog) {
            this -> rlog = &rlog;
        }

        void setup (Signal<MQTTMessage> &message) {

            this -> message = &message;

            
            
            this -> dialer.setup();
            
            this -> rlog -> log(log_prefix, "Rotary is ready");
        }

        void loop() {


            if (this -> dialer.update()) {
		        // this -> rlog -> log(log_prefix, "number:" + (String) this -> dialer.getNextNumber());
                numberToSend = numberToSend + (String) dialer.getNextNumber();
                lastGetNumberMillis = millis();
	        }

            unsigned long currentMillis = millis();
            if (numberToSend != "" && currentMillis - lastGetNumberMillis > NUMBER_TO_SEND_TIMEOUT) {                
                this -> rlog -> log(log_prefix, "Send number:" + numberToSend);

                this->message->fire(MQTTMessage{"number", numberToSend, false});

                numberToSend = "";
            }

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