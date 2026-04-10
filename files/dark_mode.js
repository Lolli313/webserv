const getCookie = (name) => {
    const value = `; ${document.cookie}`;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
}

const setCookie = (name, value, days = 30) => {
    const d = new Date();
    d.setTime(d.getTime() + (days * 24 * 60 * 60 * 1000));
    document.cookie = `${name}=${value};expires=${d.toUTCString()};path=/`;
}

function Animatedtoggle(){
    const toggle = document.querySelector(".toggle");
    toggle.classList.toggle('active');

    const isDark = document.body.classList.toggle('dark-mode');
    if (isDark)
        setCookie("theme", "dark", 30);
    else
        setCookie("theme", "light", 30);
}

window.addEventListener('DOMContentLoaded', () => {
    const savedTheme = getCookie("theme");
    const toggle = document.querySelector(".toggle");

    if (savedTheme === "dark") {
        document.body.classList.add('dark-mode');
        if (toggle) toggle.classList.add('active');
    }
})