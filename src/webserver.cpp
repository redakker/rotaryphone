#ifndef WEB
#define WEB

#include <WebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoHttpClient.h>
#include <Update.h>
#include "database.cpp"
#include "log.cpp"
#include "webcontent.h"

class Webserver {
    
    Log* rlog;
    Database* database;
    String log_prefix = "[WEB] ";
    WebServer server;
    WiFiClient* transport;
    //InternalStorageClass* InternalStorage;
    EspClass* ESP;

    public:
        Webserver (Log &log){
            this -> rlog = &log;
            WebServer server(80);

            WiFiClient transport;
        }

        void setup(Database &database) {

            this->database = &database;

            // -- Set up required URL handlers on the web server.
            // We should bind the member function in this way to able to pass to the request function.
            // https://stackoverflow.com/questions/43479328/how-to-pass-class-member-function-as-handler-function
            server.on("/", std::bind(&Webserver::handleRoot, this));

            server.on(data_functions_js_path, HTTP_GET, std::bind(&Webserver::handleJavaScript, this));
            server.on(data_style_css_path, HTTP_GET, std::bind(&Webserver::handleStyle, this));
            server.on(data_normalize_css_path, HTTP_GET, std::bind(&Webserver::handleNormalize, this));
            server.on(data_skeleton_css_path, HTTP_GET, std::bind(&Webserver::handleSkeleton, this));
            //server.on("/logo.jpg", HTTP_GET, std::bind(&Webserver::handleLogo, this));

            server.on("/data", std::bind(&Webserver::handleData, this));

            // POST
            server.on("/savedata", std::bind(&Webserver::handleSaveData, this));
            
            // update
            server.on("/update", std::bind(&Webserver::handleUpdate, this));
            // upgrade
            server.on("/upgrade", HTTP_POST, std::bind(&Webserver::handleUpgradeFn, this), std::bind(&Webserver::handleUpgradeUFn, this));

            // reset
            server.on("/reset", std::bind(&Webserver::handleReset, this));
            
            // Error handling
            server.onNotFound(std::bind(&Webserver::handleNotFound, this));

            // Favicon
            server.on("/favicon.ico", std::bind(&Webserver::handleFavicon, this));

            
            server.begin();
            this->rlog->log(log_prefix, "Webserver is ready.");
        }

        void loop(){
            server.handleClient();
        }

        String getData() {
            return this -> database -> getSerialized();
        }

    private:

        void sendHeaders(){
            server.enableCORS();
            server.enableCrossOrigin();
            server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
            server.sendHeader("Pragma", "no-cache");
            server.sendHeader("Expires","-1");
        }

        void handleRoot(){            
            this->rlog->log(log_prefix, "/ is called");
            server.sendHeader("Access-Control-Allow-Origin", "*");
            sendHeaders();
            server.send(200, "text/html", data_index_html);
        }

        void handleJavaScript(){
            this->rlog->log(log_prefix, "/function.js is called");            
            server.send(200, "text/javascript", data_functions_js);
        }

        void handleStyle(){
            this->rlog->log(log_prefix, "/style.css is called");
            server.send(200, "text/css", data_style_css);
        }

        void handleNormalize(){
            this->rlog->log(log_prefix, "/normalize.css is called");
            server.send(200, "text/css", data_normalize_css);
        }

        void handleSkeleton(){
            this->rlog->log(log_prefix, "/skeleton.css is called");
            server.send(200, "text/css", data_skeleton_css);
        }

        void handleLogo(){
            this->rlog->log(log_prefix, "/logo.jpg is called");
            //server.sendContent_P(data_logo_jpg);
        }

        void handleData(){
            this->rlog->log(log_prefix, "/data is called");
            sendHeaders();
            server.send(200, "application/json", getData());
        }

        void handleFavicon(){
            this->rlog->log(log_prefix, "/favicon is called");
            sendHeaders();
            server.send(200, "image/webp", "0");
        }

         // POST handle methods
        void handleSaveData(){
            this->rlog->log(log_prefix, (String) "/savedata is called. args: " + server.args());
            String postBody = server.arg("data");
            database->jsonToDatabase(postBody);
            sendHeaders();
            server.send(200, "application/json", getData());
            
            delay(500);
            ESP -> restart();
        }

        void handleReset() {
            this->rlog->log(log_prefix, "/reset is called");            
            String resetData = "{\"name\":\"" + (String) BOARD_NAME + "\"}";
            database->jsonToDatabase(resetData);
            server.send(200, "text/html", "Board has been reset.");
        }

        void handleUpdate() {
            this->rlog->log(log_prefix, "/update is called");
            server.send(200, "text/html", data_update_html);
        }

        void handleUpgradeFn() {
            this->rlog->log(log_prefix, "/upgrade (fn) is called");
            server.sendHeader("Connection", "close");
            server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            ESP -> restart();
        }

        void handleUpgradeUFn() {
            //this->rlog->log(log_prefix, "/upgrade (ufn) is called");
            HTTPUpload& upload = server.upload();            
            if (upload.status == UPLOAD_FILE_START) {
                this->rlog->log(log_prefix, (String) "Upload started. Upload filename: " + upload.filename.c_str());
                if (!Update.begin()) { //start with max available size
                    this->rlog->log(log_prefix, (String) "Error in update: " + Update.getError() + " -> " + Update.errorString() + " Upload status: " + upload.status);
                }
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    this->rlog->log(log_prefix, (String) "Error in update: " + Update.getError() + " -> " + Update.errorString() + " Upload status: " + upload.status);
                }
            } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) { //true to set the size to the current progress                    
                    this->rlog->log(log_prefix, (String) "Update Success. Upload size: " + upload.totalSize + " Rebooting... ");
                    // Save the uploaded filename
                    // It contains the version
                    this -> database -> updateProperty(DB_VERSION, upload.filename.c_str(), true);
                } else {
                    this->rlog->log(log_prefix, (String) "Error in update: " + Update.getError() + " -> " + Update.errorString() + " Upload status: " + upload.status);
                }
                
            } else {                
                this->rlog->log(log_prefix, (String) "Update Failed Unexpectedly (likely broken connection): " + upload.status);
            }
        }

        // 404
        void handleNotFound(){
            this->rlog->log(log_prefix, "404 is called");
            sendHeaders();
            server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
        }
};

#endif
