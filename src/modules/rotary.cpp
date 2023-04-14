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
    bool offHook = LOW;
    bool previousOffHook = LOW;
    bool readyStatus = LOW;
    String topic = PHONE_MQTT_TOPIC_ONHOOK_NUMBER;
 
    public:
        Rotary(Log &rlog) {
            this -> rlog = &rlog;
        }

        void setup (Signal<MQTTMessage> &message) {

            this -> message = &message;
            this -> dialer.setup();

            // Off-hook setip
            pinMode(PIN_OFF_HOOK, INPUT);

            this -> rlog -> log(log_prefix, "Rotary is ready");
        }

        void loop() {

            offHook = digitalRead(PIN_OFF_HOOK);
            int readyStatus = digitalRead(PIN_READY);
	        int pulseStatus = digitalRead(PIN_PULSE);


            if (this -> dialer.update(readyStatus, pulseStatus)) {
		        // this -> rlog -> log(log_prefix, "number:" + (String) this -> dialer.getNextNumber());
                numberToSend = numberToSend + (String) dialer.getNextNumber();
                lastGetNumberMillis = millis();
	        }

            unsigned long currentMillis = millis();
            // Check the readyStatus too, and you can lower the NUMBER_TO_SEND_TIMEOUT
            // otherwise there is no time to read the second number in case of timout 2000
            if (!numberToSend.isEmpty() && readyStatus == LOW && currentMillis - lastGetNumberMillis > NUMBER_TO_SEND_TIMEOUT) {                
                this -> rlog -> log(log_prefix, "Send number: " + numberToSend);

                // Decide if the off-hook is on/off
                if (offHook) {
                    topic = PHONE_MQTT_TOPIC_OFFHOOK_NUMBER;    
                } else {
                    topic = PHONE_MQTT_TOPIC_ONHOOK_NUMBER;
                }

                this->message->fire(MQTTMessage{topic, numberToSend, false});
                numberToSend = "";
            }

            // Send out the offhook/onhook event
            if (previousOffHook != offHook) {

                this -> rlog -> log(log_prefix, "Hook changed to " + (String) offHook);

                previousOffHook = offHook;

                if (offHook) {
                    this->message->fire(MQTTMessage{PHONE_MQTT_TOPIC_HOOK_EVENT, (String) 1, false});   
                } else {
                    this->message->fire(MQTTMessage{PHONE_MQTT_TOPIC_HOOK_EVENT, (String) 0, false});
                }
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