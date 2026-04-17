#include "main.hpp"
#include "Client.hpp"

int main() {
    Client client("127.0.0.1", "8080");
    bool connected = client.clientConnect();

std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
std::string body_with_quotes =
    boundary + "\r\n"
    "Content-Disposition: form-data; name=at\r\n"
    "\r\n"
    "testy\r\n"
    + boundary + "--\r\n";

std::string request_with_quotes =
    "POST / HTTP/1.1\r\n"
    "Host: 127.0.0.1:8080\r\n"
    "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
    "Content-Length: " + std::to_string(body_with_quotes.size()) + "\r\n"
    "\r\n"
    + body_with_quotes;
client.clientSend(request_with_quotes);

// std::string test =
//     "POST /api/upload HTTP/1.1\r\n"
//     "host: lOcAlHOSt\r\n"
//     "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
//     "authorization: Bearer token123\r\n"
//     "user-agent: MonClient/1.0\r\n"
//     "content-length: 583\r\n"
//     "\r\n"

//     "----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
//     "content-disposition: form-data; name=\"document\"; filename=\"rapport.abw\"\r\n"
//     "content-type: application/x-abiword\r\n"
//     "\r\n"
//     "<abiword xmlns=\"http://www.abisource.com/abiword.dtd\" version=\"1.0\">\n"
//     "  <p>Ceci est un exemple de document AbiWord.</p>\n"
//     "</abiword>\r\n"

//     "----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
//     "content-disposition: form-data; name=\"archive\"; filename=\"data.arc\"\r\n"
//     "content-type: application/x-freearc\r\n"
//     "\r\n"
//     "!<arch>\n"
//     "document.txt   1234567890  12345  100  644  1234567890  12345\n"
//     "`\n"
//     "Ceci est un exemple de contenu d'archive.\n"
//     "\n"

//     "----WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";

    // std::string test3 = "DELETE /post.json HTTP/1.1\r\n"
    // "Host: 127.0.0.1:8080\r\n\r\n";

    // std::string cookies =
    // "GET /cgi-bin/hello.py HTTP/1.1\r\n"
    // "Host: localhost:8080\r\n"
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

    
    // std::string message;

    // std::getline(std::cin, message);
    // client.clientSend(cookies);
    while (client.clientReceive().empty()) {
    }
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
