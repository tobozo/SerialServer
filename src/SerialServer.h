/*
  SerialServer.h - Dead simple Serial web-server.
  Supports only one simultaneous client, knows how to handle GET.

  cloned from ESP8266webServer by Ivan Grokhotkov 
  adapted by tobozo

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 8 May 2015 by Hristo Gochkov (proper post and file upload handling)
*/


#ifndef SerialServer_H
#define SerialServer_H

#include <functional>

enum SerialHTTPMethod { SerialHTTP_ANY, SerialHTTP_GET, SerialHTTP_POST, SerialHTTP_PUT, SerialHTTP_PATCH, SerialHTTP_DELETE, SerialHTTP_OPTIONS };
enum SerialHTTPUploadStatus { SerialUPLOAD_FILE_START, SerialUPLOAD_FILE_WRITE, SerialUPLOAD_FILE_END, SerialUPLOAD_FILE_ABORTED };

#define HTTP_DOWNLOAD_UNIT_SIZE 1460
#define HTTP_UPLOAD_BUFLEN 2048
#define HTTP_MAX_DATA_WAIT 1000 //ms to wait for the client to send the request
#define HTTP_MAX_CLOSE_WAIT 2000 //ms to wait for the client to close the connection

#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)

class SerialServer;

typedef struct {
  SerialHTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;    // file size
  size_t  currentSize;  // size of data currently in buf
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} SerialHTTPUpload;

#include "detail/RequestHandler.h"

namespace fs {
class FS;
}

class SerialServer
{
public:
  SerialServer(int serialspeed = 115200);
  ~SerialServer();

  void begin();
  void handleClient();

  typedef std::function<void(void)> THandlerFunction;
  void on(const char* uri, THandlerFunction handler);
  void on(const char* uri, SerialHTTPMethod method, THandlerFunction fn);
  void on(const char* uri, SerialHTTPMethod method, THandlerFunction fn, THandlerFunction ufn);
  void addHandler(RequestHandler* handler);
  void serveStatic(const char* uri, fs::FS& fs, const char* path, const char* cache_header = NULL );
  void onNotFound(THandlerFunction fn);  //called when handler is not assigned
  void onFileUpload(THandlerFunction fn); //handle file uploads

  String uri() { return _currentUri; }
  SerialHTTPMethod method() { return _currentMethod; }
  SerialHTTPUpload& upload() { return _currentUpload; }

  String arg(const char* name);   // get request argument value by name
  String arg(int i);              // get request argument value by number
  String argName(int i);          // get request argument name by number
  int args();                     // get arguments count
  bool hasArg(const char* name);  // check if argument exists
  void collectHeaders(const char* headerKeys[], const size_t headerKeysCount); // set the request headers to collect
  String header(const char* name);   // get request header value by name
  String header(int i);              // get request header value by number
  String headerName(int i);          // get request header name by number
  int headers();                     // get header count
  bool hasHeader(const char* name);  // check if header exists

  String hostHeader();            // get request host header if available or empty String if not

  // send response to the client
  // code - HTTP response code, can be 200 or 404
  // content_type - HTTP content type, like "text/plain" or "image/png"
  // content - actual content body
  void send(int code, const char* content_type = NULL, const String& content = String(""));
  void send(int code, char* content_type, const String& content);
  void send(int code, const String& content_type, const String& content);
  void send_P(int code, PGM_P content_type, PGM_P content);
  void send_P(int code, PGM_P content_type, PGM_P content, size_t contentLength);

  void setContentLength(size_t contentLength) { _contentLength = contentLength; }
  void sendHeader(const String& name, const String& value, bool first = false);
  void sendContent(const String& content);
  void sendContent_P(PGM_P content);
  void sendContent_P(PGM_P content, size_t size);

template<typename T> size_t streamFile(T &file, const String& contentType){
  setContentLength(file.size());
  if (String(file.name()).endsWith(".gz") &&
      contentType != "application/x-gzip" &&
      contentType != "application/octet-stream"){
    sendHeader("Content-Encoding", "gzip");
  }
  send(200, contentType, "");
  return Serial.write(file);
}

protected:
  void _addRequestHandler(RequestHandler* handler);
  void _handleRequest();
  bool _parseRequest();
  void _parseArguments(String data);
  static const char* _responseCodeToString(int code);
  bool _parseForm(String boundary, uint32_t len);
  bool _parseFormUploadAborted();
  void _uploadWriteByte(uint8_t b);
  uint8_t _uploadReadByte();
  void _prepareHeader(String& response, int code, const char* content_type, size_t contentLength);
  bool _collectHeader(const char* headerName, const char* headerValue);
  String urlDecode(const String& text);

  struct RequestArgument {
    String key;
    String value;
  };

  unsigned int  _serialspeed;
  String  _incomingSerialData;
  bool    _incomingSerialDataReady;

  SerialHTTPMethod  _currentMethod;
  String      _currentUri;

  RequestHandler*  _currentHandler;
  RequestHandler*  _firstHandler;
  RequestHandler*  _lastHandler;
  THandlerFunction _notFoundHandler;
  THandlerFunction _fileUploadHandler;

  int              _currentArgCount;
  RequestArgument* _currentArgs;
  SerialHTTPUpload       _currentUpload;

  int              _headerKeysCount;
  RequestArgument* _currentHeaders;
  size_t           _contentLength;
  String           _responseHeaders;

  String           _hostHeader;

};


#endif //SerialServer_H
