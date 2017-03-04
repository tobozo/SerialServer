#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

class RequestHandler {
public:
    virtual bool canHandle(SerialHTTPMethod method, String uri) { return false; }
    virtual bool canUpload(String uri) { return false; }
    virtual bool handle(SerialServer& server, SerialHTTPMethod requestMethod, String requestUri) { return false; }
    virtual void upload(SerialServer& server, String requestUri, SerialHTTPUpload& upload) {}

    RequestHandler* next() { return _next; }
    void next(RequestHandler* r) { _next = r; }

private:
    RequestHandler* _next = nullptr;
};

#endif //REQUESTHANDLER_H
