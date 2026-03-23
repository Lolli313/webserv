#!/usr/bin/env python3
# En-tête HTTP obligatoire
print("Content-Type: text/html\n")

# Récupère les variables d'environnement CGI
import os
import cgi
import cgitb
cgitb.enable()  # Affiche les erreurs dans le navigateur

# Récupère le paramètre "name" de l'URL (ex: ?name=Charles)
form = cgi.FieldStorage()
name = form.getvalue("name", "Invité")

# Génère la page HTML dynamiquement
print(f"""
<!DOCTYPE html>
<html>
<head>
    <title>CGI Python</title>
</head>
<body>
    <h1>Bonjour, {name} !</h1>
    <p>Ce contenu est généré par un script Python CGI.</p>
    <p>Méthode HTTP : {os.environ.get("REQUEST_METHOD", "inconnue")}</p>
    <p>Adresse IP : {os.environ.get("REMOTE_ADDR", "inconnue")}</p>
</body>
</html>
""")
