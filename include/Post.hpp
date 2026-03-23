#pragma once

#include "HttpRequest.hpp"

#include <vector>
#include <map>
#include <string>

class Post {

    private:

        const HttpRequest &_request;
        std::vector<std::map<std::string, std::string> > _header;

    public: 

        // CONSTRUCTORS

        Post(const HttpRequest &request);
        Post(const Post &other);
        ~Post();

        // FUNCTIONS

        void parseBody();
        void print() const;
        void saveInFile() const;
};
