#ifndef BLU
#define BLU

#include "BluetoothSerial.h" // Header File for Serial Bluetooth
#include "log.cpp"
#include "led.cpp"
#include <Callback.h>

class BlueTooth {

    Log* rlog;
    String log_prefix = "[BLUE] ";        
    Signal<String>* bluetoothMessageArrived;

    BluetoothSerial blueToothSerial; // Object for Bluetooth
    String command;

    public:
        BlueTooth(Log &log) {
            this -> rlog = &log;
        }

        void setup(Signal<String> &bluetoothMessageArrived) {

            this -> bluetoothMessageArrived = &bluetoothMessageArrived;
            blueToothSerial.begin(BOARD_NAME); //Name of your Bluetooth Signal
            
            // add the Blutooth serial to Log so we don't need the legacy serial anymore.
            // Logs can be saved late in the mobile app
            this->rlog->addBlueToothSerial(blueToothSerial);

            rlog -> log(log_prefix, BOARD_NAME " is ready to pair");

        }

        void loop() {
            if (blueToothSerial.available()) //Check if we receive anything from Bluetooth
            {
                command = blueToothSerial.readString(); //Read what we recevive                 
                command.trim();

                if (!command.isEmpty()) {                    
                    rlog -> log(log_prefix, "Command received: " + command);
                    this->bluetoothMessageArrived->fire(command);
                }
            }
        }
};

#endif