#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import datetime
import time

# pour tester le timeout
time.sleep(15)

# En-tête HTTP obligatoire pour CGI
print("Content-Type: text/html; charset=utf-8")
print()  # Ligne vide pour séparer les en-têtes du contenu

# Génère la page HTML avec l'heure actuelle
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI Script Execution Successful</h1>")
print(f"<p>The current server time is: <b>{datetime.datetime.now()}</b></p>")
print("</body>")
print("</html>")