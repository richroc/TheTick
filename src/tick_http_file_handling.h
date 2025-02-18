#ifndef TICK_HTTP_FILE_HANDLING_H
#define TICK_HTTP_FILE_HANDLING_H

String getContentType(String filename)
{
  if (server.hasArg("download"))
    return F("application/octet-stream");
  else if (filename.endsWith(".htm"))
    return F("text/html");
  else if (filename.endsWith(".html"))
    return F("text/html");
  else if (filename.endsWith(".css"))
    return F("text/css");
  else if (filename.endsWith(".js"))
    return F("application/javascript");
  else if (filename.endsWith(".json"))
    return F("text/json");
  else if (filename.endsWith(".png"))
    return F("image/png");
  else if (filename.endsWith(".gif"))
    return F("image/gif");
  else if (filename.endsWith(".jpg"))
    return F("image/jpeg");
  else if (filename.endsWith(".ico"))
    return F("image/x-icon");
  else if (filename.endsWith(".svg"))
    return F("image/svg+xml");
  else if (filename.endsWith(".xml"))
    return F("text/xml");
  else if (filename.endsWith(".pdf"))
    return F("application/x-pdf");
  else if (filename.endsWith(".zip"))
    return F("application/x-zip");
  else if (filename.endsWith(".gz"))
    return F("application/x-gzip");
  return "text/plain";
}

bool handleFileRead(String path)
{
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.equals(F("/")))
    path = F("/static/wiegand.html");
  if (path.endsWith(F("/")))
    path += F("index.html");
  String contentType = getContentType(path);
  String pathWithGz = path + F(".gz");
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz))
      path += F(".gz");
    File file = SPIFFS.open(path, "r");
    server.sendHeader("Now", String(millis()));
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload()
{
  if (basicAuthFailed())
    return;
  if (server.uri() != F("/edit"))
    return;
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    DBG_OUTPUT_PORT.println("handleFileUpload Name: " + filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    // DBG_OUTPUT_PORT.println("handleFileUpload Data: " + upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.println("handleFileUpload Size: " + upload.totalSize);
  }
}

void handleFileDelete()
{
  if (basicAuthFailed())
    return;
  if (server.args() == 0)
    return server.send(500, F("text/plain"), F("BAD ARGS"));
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, F("text/plain"), F("BAD PATH"));
  if (!SPIFFS.exists(path))
    return server.send(404, F("text/plain"), F("FileNotFound"));
  SPIFFS.remove(path);
  server.send(200, F("text/plain"), "");
  path = String();
}

void handleFileCreate()
{
  if (basicAuthFailed())
    return;
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList()
{
  if (basicAuthFailed())
    return;
  if (!server.hasArg("dir"))
  {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);

  File dir = SPIFFS.open(path);

  String output = "[";
  while (File file = dir.openNextFile())
  {

    if(strstr(file.path(), ".gz") != NULL){
      continue;
    }
    if(strstr(file.path(), "/static") != NULL){
      continue;
    }
    
    if (output != "[")
    {
      output += ",";
    }
    output += "{\"type\":\"";
    if (file.isDirectory())
    {
      output += "dir";
    }
    else
    {
      output += "file";
    }
    output += "\",\"name\":\"";
    output += String(file.path()).substring(1);
    output += "\"}";
  }

  output += "]";

  dir.close();
  server.send(200, "text/json", output);
}

#endif