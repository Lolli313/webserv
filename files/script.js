let selectedMethod = null;

function selectMethod(method) {
    document.querySelectorAll('.method-button').forEach(button => {
        button.classList.remove('active');
    });
    event.target.classList.add('active');
    selectedMethod = method;
}

async function validateAndSend() {
    const sendButton = document.getElementById('submit');
    const path = document.getElementById('path').value;
    const bodyText = document.getElementById('body').value;
    const fileInput = document.getElementById('file');
    const responseDiv = document.getElementById('response');

    if (!selectedMethod) {
        alert("YOU HAVE TO SELECT GET, POST or DELETE");
        return;
    }

    if (!path) {
        alert("GIVE ME A PATH");
        return;
    }

    sendButton.classList.add('active');

    try {
        let url = `http://localhost:8080${path}`;
        let options = { method: selectedMethod };

        if (selectedMethod === 'POST' && bodyText) {
            options.headers = { 'Content-Type': 'application/json' };
            options.body = bodyText;
        }

        if (fileInput.files.length > 0) {
            const formData = new FormData();
            formData.append('file', fileInput.files[0]);
            options.body = formData;
            // Ne pas définir 'Content-Type' pour FormData, le navigateur le fera automatiquement
        }

        // Envoyer la requête
        const response = await fetch(url, options);

        // Afficher la réponse dans l'encadré
        const responseText = await response.text();
        responseDiv.textContent = responseText;
    } catch (error) {
        responseDiv.textContent = `Erreur : ${error.message}`;
    } finally {
        // Retirer la classe 'active' après la fin de la requête
        setTimeout(() => {
            sendButton.classList.remove('active');
        }, 150);
    }
}
