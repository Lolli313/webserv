#pragma once

#include "main.hpp"

class Client {

    private:

        std::string _clientAddress;
        std::string _clientPort;
        int _clientSocket;
        struct addrinfo* _clientInfo;

    public:

        // CONSTRUCTORS

        Client();
        Client(const std::string& clientAddress, const std::string& clientPort);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        // GETTERS
        
        const std::string& getClientAdress() const;
        const std::string& getClientPort() const;
        int getClientSocket() const;
        bool getClientConnexion() const;

        // SETTERS

        void setClientAdress(const std::string& clientAddress);
        void setClientPort(const std::string& clientPort);

        // FUNCTIONS

        bool clientConnect();
        bool clientSend(const std::string& message);
        std::string clientReceive();
        void clientDisconnect();
};
