let uploadedFiles = [];
let selectedMethod = null;

function selectMethod(method) {
    document.querySelectorAll('.method-button').forEach(button => {
        button.classList.remove('active');
    });
    event.target.classList.add('active');
    selectedMethod = method;
}

function updateUploadsDisplay() {
    const uploadsDiv = document.getElementById('uploads');
    if (uploadedFiles.length === 0) {
        uploadsDiv.textContent = "Aucun fichier uploadé.";
        return;
    }
    let fileList = uploadedFiles.map(file => `${file.name} (${(file.size / 1024).toFixed(2)} Ko)`).join('\n');
    uploadsDiv.textContent = fileList;
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

        if (selectedMethod === 'POST') {
            if (bodyText) {
                options.headers = { 'Content-Type': 'application/json' };
                options.body = bodyText;
            }

            if (fileInput.files.length > 0) {
                const formData = new FormData();
                for (let i = 0; i < fileInput.files.length; i++) {
                    formData.append('file', fileInput.files[i]);
                    uploadedFiles.push(fileInput.files[i]); // Ajoute le fichier à la liste
                }
                options.body = formData;
                delete options.headers;
            }
        }

        const response = await fetch(url, options);
        const responseText = await response.text();
        responseDiv.textContent = `Status: ${response.status}\n${responseText}`;
        updateUploadsDisplay();
    } catch (error) {
        responseDiv.textContent = `Erreur : ${error.message}`;
    } finally {
        // Retirer la classe 'active' après la fin de la requête
        setTimeout(() => {
            sendButton.classList.remove('active');
        }, 150);
    }
}
