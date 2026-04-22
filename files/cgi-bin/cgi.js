
// Run this when the page loads
window.onload = async function() {
    await loadCgiScripts();
};

async function loadCgiScripts() {
    const listElement = document.getElementById('cgiList');
    
    try {
        // 1. Fetch the JSON your C++ server already generates
        const response = await fetch('/cgi-bin/?format=json'); // Adjust URL to your autoindex route
        const files = await response.json(); // Assumes files is an array of strings or objects

        // 2. Clear the list
        listElement.innerHTML = '';

        // 3. Map the JSON data to HTML elements
        files.forEach(file => {
            // If your JSON is an object like {name: "test.py"}, use file.name
            const fileName = typeof file === 'string' ? file : file.name;

            // Skip hidden files if your C++ doesn't already
            if (fileName.startsWith('.')) return;

            const isScript = file.name.endsWith('.py') || file.name.endsWith('.php') || file.name.endsWith('.sh');
            if (!isScript) return;

            const li = document.createElement('li');
            li.className = 'cgi-item';
            li.innerHTML = `
                <span class="cgi-name">${fileName}</span>
                <button onclick="executeCGI('${fileName}')">EXECUTE</button>
            `;
            listElement.appendChild(li);
        });

    } catch (err) {
        listElement.innerHTML = `<li>Error loading scripts: ${err.message}</li>`;
    }
}

async function executeCGI(scriptName) {
    const output = document.getElementById('output');
    output.textContent = `> Executing ${scriptName}...`;
    
    try {
        const response = await fetch(`/cgi-bin/${scriptName}`);
        const result = await response.text();
        output.textContent = result;
    } catch (err) {
        output.textContent = "Error executing script.";
    }
}