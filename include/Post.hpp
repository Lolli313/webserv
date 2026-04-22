#pragma once

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "ConfigBase.hpp"

#include <vector>
#include <string>
#include <map>

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
        void saveInFile(const HttpRequest &request, const ConfigBase *config) const;

        static const std::string executePost(const HttpRequest &request, const ConfigBase *config);

};