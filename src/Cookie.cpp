#include "Cookie.hpp"
#include "TerminalColors.hpp"

#include <sstream>

/*
================================================================================
======= CONSTRUCTORS // DESTRUCTOR =============================================
================================================================================
*/

Cookie::Cookie() : _cookie() {}

Cookie::~Cookie() {}

/*
================================================================================
======= SETTERS ================================================================
================================================================================
*/

void Cookie::setCookie(std::string cookie) {
    std::string line;
    std::stringstream ss(cookie);
    std::map<std::string, std::string> tmp;

    // isole chaque sous-cookie du cookie general pour le stocker dans la variable tmp
    while (std::getline(ss, line, ';')) {
        size_t posEqual = line.find('=');
        if (posEqual != std::string::npos) {
            std::string key = line.substr(0, posEqual);
            std::string value = line.substr(posEqual + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (!key.empty()) {
                tmp[key] = value;
            }
        }
    }

    // retrouve le session_id pour savoir si on a deja des cookies sur lui
    // il ecrase les anciennes key ou en creer des nouvelles
    std::map<std::string, std::string>::const_iterator itTmpId = tmp.find("session_id");
    if (itTmpId == tmp.end()) {
        return;
    }
    std::string sessionId = itTmpId->second;
    std::map<std::string, std::map<std::string, std::string> >::iterator itId = _cookie.find(sessionId);
    if (itId != _cookie.end()) {
        for (std::map<std::string, std::string>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
            itId->second[it->first] = it->second;
        }
    } else {
        _cookie[sessionId] = tmp;
    }
}

/*
================================================================================
======= FUNCTIONS ==============================================================
================================================================================
*/

void Cookie::printCookie() const {
    for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _cookie.begin(); it != _cookie.end(); ++it) {
        std::cout << YELLOW << it->first << RESET << std::endl;
        for (std::map<std::string, std::string>::const_iterator itSession = it->second.begin(); itSession != it->second.end(); ++itSession) {
            std::cout << " " << itSession->first << " : " << itSession->second << std::endl;
        }
    }
}
