#ifndef SERIALREQUESTHANDLER_H
#define SERIALREQUESTHANDLER_H

class SerialRequestHandler {
public:
    virtual bool canHandle(SerialHTTPMethod method, String uri) { return false; }
    virtual bool canUpload(String uri) { return false; }
    virtual bool handle(SerialServer& server, SerialHTTPMethod requestMethod, String requestUri) { return false; }
    virtual void upload(SerialServer& server, String requestUri, SerialHTTPUpload& upload) {}
    virtual String getUri() { return ""; }

    SerialRequestHandler* next() { return _next; }
    void next(SerialRequestHandler* r) { _next = r; }

private:
    SerialRequestHandler* _next = nullptr;
};

#endif //SERIALREQUESTHANDLER_H
