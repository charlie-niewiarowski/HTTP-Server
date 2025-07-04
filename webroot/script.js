document.addEventListener("DOMContentLoaded", () => {
  fetch("data.json")
    .then(response => response.json())
    .then(data => {
      document.getElementById("message").textContent = data.message;
    });
});
