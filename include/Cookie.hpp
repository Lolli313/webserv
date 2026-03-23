#pragma once

#include <map>
#include <string>
#include <iostream>

class Cookie {

    private:

        std::map<std::string, std::map<std::string, std::string> > _cookie;

    public:

        Cookie();
        ~Cookie();

        void setCookie(std::string cookie);

        void printCookie() const;
};
