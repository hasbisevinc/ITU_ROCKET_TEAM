window.showRaw = function() {
    var rawContainer = document.getElementById("rawContainer");
    var mapContainer = document.getElementById("map");
    mapContainer.classList.add("hide-container");
    rawContainer.classList.remove("hide-container");
}

window.showMap = function() {
    var rawContainer = document.getElementById("rawContainer");
    var mapContainer = document.getElementById("map");
    rawContainer.classList.add("hide-container");
    mapContainer.classList.remove("hide-container");
}
