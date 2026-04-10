let uploadedFiles = [];
let selectedMethod = null;

/**
 * Sélectionne la méthode HTTP
 */
function selectMethod(method, event) {
    document.querySelectorAll('.method-button').forEach(button => {
        button.classList.remove('active');
    });
    event.target.classList.add('active');
    selectedMethod = method;
}

/**
 * Réinitialise le champ de fichier
 */
function resetFileInput() {
    const fileInput = document.getElementById('file');
    fileInput.value = '';
    uploadedFiles = [];
    updateUploadsDisplay();
}

/**
 * Efface le contenu du body
 */
function clearBody() {
    document.getElementById('json').value = '';
}

/**
 * Met à jour l'affichage des fichiers uploadés
 */
function updateUploadsDisplay() {
    const uploadsDiv = document.getElementById('uploads');
    if (uploadedFiles.length === 0) {
        uploadsDiv.textContent = '';
        return;
    }
    let fileList = uploadedFiles.map(file => `${file.name} (${(file.size / 1024).toFixed(2)} Ko)`).join('\n');
    uploadsDiv.textContent = fileList;
}

/**
 * Valide et envoie la requête HTTP
 */
async function validateAndSend() {
    const submitButton = document.getElementById('submit');
    const path = document.getElementById('path').value.trim();
    const name = document.getElementById('name').value.trim();
    const bodyText = document.getElementById('json').value.trim();
    const fileInput = document.getElementById('file');
    const responseDiv = document.getElementById('response');

    // Vérifie qu'une méthode est sélectionnée
    if (!selectedMethod) {
        alert("Veuillez sélectionner une méthode (GET, POST ou DELETE).");
        return;
    }

    // Vérifie qu'un path est saisi
    if (!path) {
        alert("Veuillez saisir un path.");
        return;
    }

    // Pour POST, vérifie que le name est obligatoire
    if (selectedMethod === 'POST' && !name) {
        alert("Veuillez saisir un nom pour la requête POST.");
        return;
    }

    // Pour POST, vérifie qu'un body.json OU un fichier est fourni (mais pas les deux)
    if (selectedMethod === 'POST') {
        if (!bodyText && fileInput.files.length === 0) {
            alert("Veuillez fournir soit un body.json, soit un fichier.");
            return;
        }
        if (bodyText && fileInput.files.length > 0) {
            const useBody = confirm("Un body.json et un fichier sont fournis. Voulez-vous utiliser le body.json et ignorer le fichier ?");
            if (useBody) {
                resetFileInput(); // Réinitialise le champ de fichier
            } else {
                clearBody(); // Efface le body
                alert("Le body.json a été effacé. Seul le fichier sera envoyé.");
                return; // Arrête l'exécution pour forcer l'utilisateur à refaire son choix
            }
        }
    }

    submitButton.classList.add('active');

    try {
        let url = `http://localhost:8080${path}`;
        let options = { method: selectedMethod };

        // Configuration spécifique pour POST
        if (selectedMethod === 'POST') {
            if (bodyText) {
                // Envoi du body.json comme texte brut
                options.headers = { 'Content-Type': 'application/json' };
                options.body = bodyText; // Envoie le contenu brut du body
            } else if (fileInput.files.length > 0) {
                // Envoi du fichier avec le nom du champ personnalisé (name)
                const formData = new FormData();
                const file = fileInput.files[0];
                formData.append(name, file, file.name); // Utilise le nom original du fichier
                uploadedFiles = [file];
                options.body = formData;
            }
        }

        // Envoi de la requête
        const response = await fetch(url, options);
        const responseText = await response.text();

        // Affichage de la réponse en texte brut
        responseDiv.textContent = `Statut : ${response.status}\n${responseText}`;
        updateUploadsDisplay();
    } catch (error) {
        responseDiv.textContent = `Erreur : ${error.message}`;
    } finally {
        setTimeout(() => {
            submitButton.classList.remove('active');
        }, 150);
    }
}
