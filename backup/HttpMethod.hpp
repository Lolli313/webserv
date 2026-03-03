// GET,Récupère une ressource (ex: une page HTML, une image).
// POST,Envoie des données à traiter (ex: formulaire, upload de fichier).
// PUT,Met à jour une ressource existante ou la crée si elle n'existe pas.
// DELETE,Supprime une ressource.
// HEAD,Similaire à GET, mais ne retourne que les en-têtes (pas le corps).
// OPTIONS,Retourne les méthodes HTTP supportées par le serveur pour une URL donnée.
// PATCH,Applique des modifications partielles à une ressource.
// TRACE,Retourne la requête reçue (utile pour le débogage).
// CONNECT,Utilisé pour établir un tunnel (ex: HTTPS via proxy).

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cstring>
#include <stdexcept>

class HttpMethod {
    private:

        std::string _method;
        std::string _path;
        std::map<std::string, std::string> _header;
        std::string _body;

        void parseRequest(const std::string &request);
        
    public:

        HttpMethod();
        HttpMethod(const std::string &request);
        ~HttpMethod();

        void parse(const std::string &request);
        const std::string &getMethod() const;
        const std::string &getPath() const;
        const std::map<std::string, std::string> &getHeader() const;
        const std::string &getBody() const;
};
