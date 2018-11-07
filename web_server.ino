void web_server_setup() {  
  www_server.on("/", handle_root);
  www_server.on("/update_form", handle_update_form);
  www_server.on("/update",HTTP_POST, handle_update, handle_update_upload);
  www_server.begin();

  Serial.println("Web server started");
}

void handle_root() {

  String html = pre_main + root_main + post_main;
  
  www_server.sendHeader("Connection", "close");
  www_server.sendHeader("Access-Control-Allow-Origin", "*");
  www_server.send(200, "text/html", html);
}

void handle_update_form(){
  
  String form = "<form method='POST' action='/update' enctype='multipart/form-data'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
  "</form>";

  String html = pre_main + form + post_main;
  www_server.sendHeader("Connection", "close");
  www_server.sendHeader("Access-Control-Allow-Origin", "*");
  www_server.send(200, "text/html", html);
}

void handle_update(){

  String upload_status;
  if(Update.hasError()){
    upload_status = "Upload failed";
  }
  else {
    upload_status = "Upload success";
  }
  String html = pre_main + upload_status + post_main;
  www_server.sendHeader("Connection", "close");
  www_server.sendHeader("Access-Control-Allow-Origin", "*");
  www_server.send(200, "text/html", html);
  ESP.restart();
  
}

void handle_update_upload() {
  HTTPUpload& upload = www_server.upload();
  if(upload.status == UPLOAD_FILE_START){
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if(!Update.begin(maxSketchSpace)){//start with max available size
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}
