#include "main.hpp"
#include "Client.hpp"

int main() {
    Client client("127.0.0.1", "8080");
    bool connected = client.clientConnect();

    // std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";

    // std::string request =
    //     "POST /upload HTTP/1.1\r\n"
    //     "Host: 127.0.0.1:8080\r\n"
    //     "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
    //     "Content-Length: 250\r\n"
    //     "\r\n"
    //     + boundary + "\r\n"
    //     "Content-Disposition: form-data; name=\"data\"\r\n"
    //     "\r\n"
    //     "testy\r\n"
    //     + boundary + "--\r\n";
    // client.clientSend(request);

    std::string test =
    "POST /api/utilisateurs HTTP/1.1\r\n"
    "Host: 127.0.0.1:8080\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 71\r\n"
    "Authorization: Bearer token123\r\n"
    "User-Agent: MonClient/1.0\r\n"
    "\r\n"
    "{\n"
    "  \"nom\": \"Charles\",\n"
    "  \"age\": 30,\n"
    "  \"email\": \"charles@example.com\"\n"
    "}";

    // std::string test3 = "DELETE /post.json HTTP/1.1\r\n"
    // "Host: 127.0.0.1:8080\r\n\r\n";

    // std::string cookies =
    // "GET /hello.txt HTTP/1.1\r\n"
    // "Host: cgi.com\r\n"
    // "User-Agent: Mozilla/5.0 (X11; Linux x86_64)\r\n"
    // "Accept: text/html\r\n"
    // "Cookie: session_id=abc123; theme=dark; lang=fr\r\n"
    // "Connection: keep-alive\r\n"
    // "\r\n";


    // std::string boulangerie =
    // "GET /dashboard HTTP/1.1\r\n"
    // "Host: cgi.com\r\n"
    // "User-Agent: Mozilla/5.0 (X11; Linux x86_64)\r\n"
    // "Accept: text/html\r\n"
    // "Cookie: session_id=abc123; theme=light; test=3\r\n"
    // "Connection: keep-alive\r\n"
    // "\r\n";

    // std::string cgi =
    // "GET cgi-bin/hello.py?name=Alice&age=30 HTTP/1.1\r\n"
    // "Host: localhost:8080\r\n"
    // "User-Agent: curl/7.68.0\r\n"
    // "Accept: */*\r\n"
    // "Connection: close\r\n"
    // "\r\n";

    
    std::string message;

    std::getline(std::cin, message);
    client.clientSend(test);
    // client.clientSend(test2);
    // std::getline(std::cin, message);
    // client.clientSend(test3);
    // client.clientSend(test2);
    // std::getline(std::cin, message);
    // client.clientSend(cookies);
    // std::getline(std::cin, message);
    // client.clientSend(boulangerie);
    // std::getline(std::cin, message);
    // client.clientSend(test3);
    // std::getline(std::cin, message);
    client.clientDisconnect();
    return 0;
}
