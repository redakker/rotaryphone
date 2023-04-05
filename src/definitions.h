// VERSION
#define MAJOR_VERSION 0.60

// Board specific setings
// board name which will be verified in its database. If does not match with the database, db will be reset
#define BOARD_NAME "rotaryphone"
// What is this board for? Bed sensor? Relay? Give any kind of name here which will apeear on the web interface
#define APP_NAME "Rotary phone"
#define LED_BUILTIN 2
#define EEPROM_SIZE 1024

// Software settings
#define SERVER_PORT 80
#define MQTT_MAX_TRY 10 // give the connect up after this amount of tries
#define MQTT_TOPIC "/dice"
#define MQTT_IN_POSTFIX "/in"
#define MQTT_STATUS_ON "{\"status\": \"on\"}"
#define MQTT_STATUS_OFF "{\"status\": \"off\"}"

// Brightness control is skipped under this animation speed
#define BRIGHTNESS_CONTROL_TIME_LIMIT 400


// Network
#define WIFI_MAX_TRY 10
#define AP_IP {192, 168, 4, 1} // Change together with the string version
#define AP_IP_STRING "192.168.4.1" // Change together with the object version
#define AP_NETMASK {255, 255, 255, 0}

// ERRORS
#define ERROR_NO_ERROR 0
#define ERROR_UNKNOWN 1
#define ERROR_WIFI 2
#define ERROR_MQTT 8

#define COMMAND_CONFIG "config"

// DATABASE PROPERTIES
#define DB_WIFI_NAME "ssid"
#define DB_WIFI_PASSWORD "pw"
#define DB_MQTT_SERVER "mqttserver"
#define DB_MQTT_PORT "mqttport"
#define DB_MQTT_USER "mqttuser"
#define DB_MQTT_PW "mqttpw"
#define DB_MQTT_TOPIC_PREFIX "mqttprefix"
#define DB_VERSION "version"
#define DB_DETAILED_REPORT "detailed"
#define DB_REBOOT_TIMEOUT "reboot"
