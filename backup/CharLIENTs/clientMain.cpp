#include "main.hpp"
#include "Client.hpp"

int main() {
    Client client("127.0.0.1", "8080");
    bool connected = client.clientConnect();

    std::string test =
    "POST /api/utilisateurs HTTP/1.1\r\n"
    "Host: exemple.com\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 71\r\n"
    "Authorization: Bearer token123\r\n"
    "User-Agent: MonClient/1.0\r\n";

    std::string test2 =
    "\r\n"
    "{\n"
    "  \"nom\": \"Charles\",\n"
    "  \"age\": 30,\n"
    "  \"email\": \"charles@example.com\"\n"
    "}\r\n";

    std::string test3 =
    "DELETE files/post.json HTTP/1.1\r\n";

    client.clientSend(test);
    client.clientSend(test2);
    while(true) {
        std::string message;
        std::getline(std::cin, message);
        client.clientSend(test3);
    }
    client.clientDisconnect();
    return 0;
}


