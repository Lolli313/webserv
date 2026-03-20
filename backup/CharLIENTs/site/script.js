document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('form');
    const methodSelect = document.getElementById('method');
    const pathInput = document.getElementById('path');
    const bodyTextarea = document.getElementById('body');
    const fileInputGroup = document.getElementById('post-file-group');
    const fileInput = document.getElementById('file');
    const responseText = document.getElementById('response-text');

    // Affiche/masque les options POST selon la méthode choisie
    methodSelect.addEventListener('change', () => {
        const method = methodSelect.value;
        if (method === 'POST') {
            fileInputGroup.style.display = 'block';
        } else {
            fileInputGroup.style.display = 'none';
        }
    });

    // Gestion de l'envoi du formulaire
    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        responseText.textContent = "Envoi en cours...";

        const method = methodSelect.value;
        const path = pathInput.value;
        let body = bodyTextarea.value;

        // Pour POST avec fichier
        if (method === 'POST' && fileInput.files.length > 0) {
            const file = fileInput.files[0];
            body = await file.text();
        }

        // Utilisation de FormData pour éviter OPTIONS
        const formData = new FormData();
        if (method === 'POST' && body) {
            formData.append('data', body);
        }

        // Options pour fetch (sans en-têtes personnalisés)
        const options = {
            method: method,
            body: method === 'POST' ? formData : undefined,
            // Pas d'en-têtes personnalisés pour éviter OPTIONS
        };

        try {
            // Envoi de la requête
            const response = await fetch(`http://127.0.0.1:8080${path}`, options);
            const text = await response.text();
            responseText.textContent = text;
        } catch (error) {
            responseText.textContent = `Erreur : ${error.message}`;
        }
    });
});
