#pragma once

#include "HttpRequest.hpp"

class Post {

    private:

        HttpRequest _request;
        std::vector<std::map<std::string, std::string> > _header;

    public: 

        // CONSTRUCTORS

        Post();
        Post(HttpRequest &request);
        Post(const Post &other);
        Post &operator=(const Post &other);
        ~Post();

        // FUNCTIONS

        void parseBody();
        void print() const;
        void saveInFile() const;
};
