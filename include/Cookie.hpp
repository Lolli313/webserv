#pragma once

#include "TerminalColors.hpp"
#include "Tools.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <map>

class Cookie {

    private:

        std::map<std::string, std::map<std::string, std::string> > _cookie;

    public:

        Cookie();
        ~Cookie();

        void setCookie(std::string cookie);

        void printCookie() const;
};
