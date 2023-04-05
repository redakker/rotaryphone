#ifndef WIFI
#define WIFI

#include <WiFi.h>
#include "WiFiUdp.h"
#include <Callback.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "log.cpp"
#include "database.cpp"

class Wifi {

    public:

        Log* rlog;
        Signal<boolean>* wifiStatusChanged;
        Signal<int>* errorCodeChanged;
        String log_prefix = "[WIFI] ";
        bool wifi_connected = false;
        // DNSServer dnsServer; 
        Database* database;
        DNSServer dnsServer; 
        

        String ssid;
        String password;

        int tries = 0;

        Wifi(Log &log) {
            this -> rlog = &log;
        }

        void setup(Database &database, Signal<boolean> &wifiStatusChanged, Signal<int> &errorCodeChanged){

            this -> database = &database;

            this -> ssid = this -> database -> getValueAsString(String(DB_WIFI_NAME), false);
            this -> password = this -> database -> getValueAsString(String(DB_WIFI_PASSWORD), false);

            this -> wifiStatusChanged = &wifiStatusChanged;
            this -> errorCodeChanged = &errorCodeChanged;

            WiFi.onEvent(
            [this](WiFiEvent_t event, system_event_info_t info) {
                this->WiFiEvent(event);
            });
            
            configAP();
            WiFi.mode(WIFI_AP_STA);

            setupMDNS();
        }

        void connectWifi() {            
            if (ssid.length() > 0) {
                this -> connectToAP();
            } else {
                rlog -> log(log_prefix, "Cannot connect to wifi, because no SSID was defined. Create an AP.");
                createAP();
            }
            
        }

        void disconnectWifi() {
            WiFi.disconnect();
            rlog -> log(log_prefix, "Wifi is disconnected from a function.");
        }

        void connectToAP() {
            WiFi.mode(WIFI_STA);
            WiFi.begin(const_cast<char*>(ssid.c_str()), const_cast<char*>(password.c_str()));            
        }

        void createAP() {
            configAP();
            WiFi.mode(WIFI_AP);
            rlog -> log(log_prefix + "AP is created from a function. Name: " + BOARD_NAME);
        }

        void stopAP(){            
            WiFi.softAPdisconnect();
            WiFi.enableAP(false);
            rlog -> log(log_prefix, "AP disconnected from a function.");           
        }

        boolean isConnected(){
            return wifi_connected;
        }     

        void loop(){
            if(wifi_connected){
                wifiConnectedLoop();
            } else {
                wifiDisconnectedLoop();
                // dnsServer.processNextRequest();
            }
            
        }


    private:

        void WiFiEvent(WiFiEvent_t event){
            switch(event) {

                case SYSTEM_EVENT_AP_START:
                    //can set ap hostname here
                    WiFi.softAPsetHostname(BOARD_NAME);
                    //enable ap ipv6 here
                    // WiFi.softAPenableIpV6();

                    rlog -> log(log_prefix + "AP started. SSID: " + BOARD_NAME + " AP IPv4: " + WiFi.softAPIP().toString());
                    break;

                case SYSTEM_EVENT_STA_START:
                    //set sta hostname here
                    WiFi.setHostname(BOARD_NAME);
                    rlog -> log(log_prefix + "Wifi hostname set to " + BOARD_NAME);
                    break;
                case SYSTEM_EVENT_STA_CONNECTED:
                    // enable sta ipv6 here
                    // WiFi.enableIpV6();
                    break;
                case SYSTEM_EVENT_AP_STA_GOT_IP6:
                    //both interfaces get the same event
                    // log -> log("STA IPv6: ");
                    // log -> log(WiFi.localIPv6());
                    // log -> log("AP IPv6: ");
                    // log -> log(WiFi.softAPIPv6());
                    break;
                case SYSTEM_EVENT_STA_GOT_IP:
                    wifiOnConnect();
                    break;
                case SYSTEM_EVENT_STA_DISCONNECTED:                                       
                    wifiOnDisconnect();
                    break;
                default:
                    break;
            }
        }

        // when wifi connects
        void wifiOnConnect() {
            tries = 0;            
            stopAP();
            wifi_connected = true;
            
            // Emit a NO_ERROR event about the Wifi status
            errorCodeChanged->fire(ERROR_NO_ERROR);
            
            // Emit an event about the Wifi status
            wifiStatusChanged->fire(wifi_connected);
            rlog -> log(log_prefix, "STA Connected. STA SSID: " + WiFi.SSID() + " STA IPv4: " + WiFi.localIP().toString() + ", GW: " + WiFi.gatewayIP().toString()+ ", Mask: " + WiFi.subnetMask().toString() + ", DNS: " + WiFi.dnsIP().toString());
        }

        // when wifi disconnects
        void wifiOnDisconnect() {
            rlog -> log(log_prefix, "Disconnected.");
            wifi_connected = false;
            
            // Emit an event about the Wifi status
            wifiStatusChanged->fire(wifi_connected);
            
            // Try agan till WIFI_MAX_TRY
            this->tries++;
            if (this->tries > WIFI_MAX_TRY) {
                WiFi.disconnect();
                rlog -> log(log_prefix, "Final disconnect.");
                errorCodeChanged->fire(ERROR_WIFI);
                this -> createAP();
            } else {
                this -> connectToAP();
            }
            //this -> connectWifi();
        }

        // while wifi is connected
        void wifiConnectedLoop() {            
        }

        // while wifi is not connected
        void wifiDisconnectedLoop() {
            // Try to reconnect time to time
            // TODO
        }

        void setupMDNS() {
            if(!MDNS.begin( BOARD_NAME )) {
                rlog -> log(log_prefix, "Error starting mDNS");
                //return;
            } else {
                MDNS.addService("http", "tcp", 80);
            }
        }

        void configAP() {
            WiFi.softAP(BOARD_NAME);

            // For captive portal
            dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
            dnsServer.start(53, "*", AP_IP);

            WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
        }

};

#endif
