document.getElementById("uploadForm").addEventListener("submit", async (event) => {
    event.preventDefault();

    const fileInput = document.getElementById("fileInput");
    const file = fileInput.files[0];

    if (!file) {
        document.getElementById("response").innerText = "Aucun fichier sélectionné.";
        return;
    }

    const formData = new FormData();
    formData.append("fichier", file);

    try {
        const response = await fetch("/api/upload", {
            method: "POST",
            body: formData,
        });

        if (!response.ok) {
            throw new Error(`Erreur HTTP : ${response.status}`);
        }

        const result = await response.text();
        document.getElementById("response").innerHTML = `
            <h3>Réponse du serveur :</h3>
            <pre>${result}</pre>
        `;
    } catch (error) {
        document.getElementById("response").innerText =
            `Erreur : ${error.message}.`;
    }
});
