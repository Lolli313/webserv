#!/usr/bin/env python3
# Ce script doit être exécutable : chmod +x hello.py

# En-tête HTTP obligatoire pour les CGI
print("Content-Type: text/html\n")

# Récupère les variables d'environnement CGI
import os
import datetime

# Récupère le paramètre "name" de l'URL (ex: ?name=Charles)
query_string = os.getenv("QUERY_STRING", "")
params = {}
if query_string:
    for pair in query_string.split("&"):
        key, value = pair.split("=")
        params[key] = value

name = params.get("name", "Invité")

# Génère la page HTML dynamiquement
html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Exemple CGI</title>
</head>
<body>
    <h1>Bonjour, {name} !</h1>
    <p>Il est actuellement {datetime.datetime.now().strftime("%H:%M:%S")}.</p>
    <p>Ton adresse IP est : {os.getenv("REMOTE_ADDR", "inconnue")}</p>
    <p>Méthode HTTP utilisée : {os.getenv("REQUEST_METHOD", "inconnue")}</p>
</body>
</html>
"""
print(html)
