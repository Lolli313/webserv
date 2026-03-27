#include "Client.hpp"

/*
================================================================================
======= CONSTRUCTORS // DESTRUCTOR =============================================
================================================================================
*/

Client::Client()
    : _clientAddress(""), _clientPort(""), _clientSocket(-1), _clientInfo(NULL) {}

Client::Client(const std::string& clientAddress, const std::string& clientPort)
    : _clientAddress(clientAddress), _clientPort(clientPort), _clientSocket(-1), _clientInfo(NULL) {}

Client::Client(const Client& other)
    : _clientAddress(other._clientAddress), _clientPort(other._clientPort), _clientSocket(-1), _clientInfo(NULL) {}

Client& Client::operator=(const Client& other) {
    if (this != &other) {
        _clientAddress = other._clientAddress;
        _clientPort = other._clientPort;
        clientDisconnect();
        _clientSocket = -1;
        _clientInfo = NULL;
    }
    return *this;
}

Client::~Client() {
    clientDisconnect();
}

/*
================================================================================
======= GETTERS ================================================================
================================================================================
*/

const std::string& Client::getClientAdress() const {
    return _clientAddress;
}

const std::string& Client::getClientPort() const {
    return _clientPort;
}

int Client::getClientSocket() const {
    return _clientSocket;
}

bool Client::getClientConnexion() const {
    return _clientSocket != -1;
}

/*
================================================================================
======= SETTERS ================================================================
================================================================================
*/

void Client::setClientAdress(const std::string& clientAdress) {
    _clientAddress = clientAdress;
}

void Client::setClientPort(const std::string& clientPort) {
    _clientPort = clientPort;
}

/*
================================================================================
======= FUNCTIONS ==============================================================
================================================================================
*/

bool Client::clientConnect() {

    if (_clientAddress.empty() || _clientPort.empty()) {
        std::cerr << RED << "CLIENT ADRESS AND PORT ERROR" << RST << std::endl;
        return false;
    }

    // addrinfo stock toutes les informations pour une socket
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); // initialisation a 0;
    hints.ai_family = AF_UNSPEC; // IPV4 et IPV6
    hints.ai_socktype = SOCK_STREAM; // Transmission Control Protocol

    // resout le nom, de l'hote ainsi que le port en une liste d'adresses
    if (getaddrinfo(_clientAddress.c_str(), _clientPort.c_str(), &hints, &_clientInfo) != 0) {
        std::cerr << RED << "CLIENT GETADDRINFO ERROR" << RST << std::endl;
        return false;
    }

    if ((_clientSocket = socket(_clientInfo->ai_family, _clientInfo->ai_socktype, _clientInfo->ai_protocol)) == -1) {
        std::cerr << RED << "CLIENT SOCKET ERROR" << std::endl;
        freeaddrinfo(_clientInfo);
        _clientInfo = NULL;
        return false;
    }

    if (connect(_clientSocket, _clientInfo->ai_addr, _clientInfo->ai_addrlen) == -1) {
        std::cerr << RED << "CLIENT CONNECTION ERROR" << RST << std::endl;
        close(_clientSocket);
        _clientSocket = -1;
        freeaddrinfo(_clientInfo);
        _clientInfo = NULL;
        return false;
    }
    
    std::clog << GRN << "client connected" << RST << std::endl;
    return true;
}

bool Client::clientSend(const std::string& message) {

    if (_clientSocket == -1) {
        std::cerr << RED << "CLIENT UNCONNECTED SOCKET" << RST << std::endl;
        return false;
    }

    ssize_t bytesSent = send(_clientSocket, message.c_str(), message.size(), 0);

    if (bytesSent == -1) {
        std::cerr << RED << "CLIENT SEND ERROR" << RST << std::endl;
        return false;
    }

    std::clog << GRN << "client sent message" << RST << std::endl;
    return true;
}



std::string Client::clientReceive() {

    if (_clientSocket == -1) {
        std::cerr << RED << "CLIENT UNCONNECTED SOCKET" << RST << std::endl;
        return "";
    }

    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string message;
    ssize_t bytesReceived;

    int flags = fcntl(_clientSocket, F_GETFL, 0);
    fcntl(_clientSocket, F_SETFL, flags | O_NONBLOCK);

    while ((bytesReceived = recv(_clientSocket, buffer, bufferSize - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        message += buffer;
    }

    if (bytesReceived == 0) {
        std::clog << GRN << "[client] connection closed by server" << RST << std::endl;
        return "";
    } else if (bytesReceived < 0) {
        // std::cerr << RED << "CLIENT RECV ERROR" << RST << std::endl;
        return "";
    }

    std::clog << GRN << "client received message" RST << std::endl;
    return message;
}

void Client::clientDisconnect() {
    
    if (_clientSocket != -1) {
        if (close(_clientSocket) == -1) {
            std::cerr << RED << "CLIENT CLOSE SOCKET ERROR" << RST << std::endl;
        } else {
            std::clog << GRN << "client socket closed" << RST << std::endl;
        }
        _clientSocket = -1;
    }

    if (_clientInfo != NULL) {
        freeaddrinfo(_clientInfo);
        _clientInfo = NULL;
        std::clog << GRN << "client info freed" << RST << std::endl;
    }

    std::clog << GRN << "client disconnected" << RST << std::endl;
}
