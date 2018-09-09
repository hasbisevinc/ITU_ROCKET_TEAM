// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

var SerialPort = require('serialport');
var port = new SerialPort('XXX');
var connectedPort = "";
var fileName = "";

var bufferedString = "";


window.refreshComPortList = function() {
  SerialPort.list(function (err, ports) {
    clearComPortList();
    ports.forEach(function(port) {
      console.log(port.comName);
      addOptionsToComPortList(port.comName);
    });
  });
}

window.connectButtonEvent = function () {
  if (connectedPort == "") {
    connect(getSelectedPort(), getSelectedBaudRate());
  } else {
    disconnect();
  }
}

window.openDragParachute = function () {
  send("XDY");
}

window.openMainParachute = function () {
  send("XMY");
}

window.rereferencing = function () {
  send("XRY");
}

initPage();

function initPage() {
  window.refreshComPortList();
}

function appendToRawData(message) {
    rawData = document.getElementById("rawData");
    rawData.append(message);
}

function onMessage(message) {
    window.messageHandler(message);
}

function getSelectedPort() {
  var comports = document.getElementById("comports");
  return comports.options[comports.selectedIndex].value;
}

function getSelectedBaudRate() {
  var baudRates = document.getElementById("baudRates");
  return parseInt(baudRates.options[baudRates.selectedIndex].value);
}

function clearComPortList() {
  var comports = document.getElementById("comports");
  while( 0 < comports.options.length) {
    comports.remove(0);
  }
}

function generateFileName() {
  var d = new Date();
  var file =  d.getDate() + "_" + (d.getMonth() + 1) + "_" + d.getFullYear() + "__" + d.getHours() + "_" + d.getMinutes() + "_" + d.getSeconds() + ".txt";
  return file;
}

function writeToFile(message) {
  var fs = require('fs');
  fs.appendFile(fileName, message, function(err) {
      if(err) {
          return console.log(err);
      }
  
      console.log("The file was saved!");
  }); 
}

function addOptionsToComPortList(comport) {
  var option = document.createElement("option");
  option.text = comport;
  option.value = comport;
  comports.add(option);
}

function connect(comport, baudRateValue) {
  port = new SerialPort(comport, {
    baudRate: baudRateValue
  });
  port.on('open', function() {
    console.log('serial port open');
    document.getElementById("connectButton").innerHTML = "Disconnect"
    connectedPort = comport;
    fileName = generateFileName();
  });
 
  port.on('error', function(err) {
    console.log('Error: ', err.message);
    window.showErrorDialog("Port Error:"+err.message)
  })

  port.on('data', function (data) {
    data = data + "";
    appendToRawData(data);
    writeToFile(data);
    if (data.lastIndexOf("\n") != -1)
    {
      buf = data.substring(
        data.lastIndexOf("\n") +1, 
        data.length -1
      )
      current = data.substring(
        0, 
        data.lastIndexOf("\n")
      )
      onMessage(bufferedString + current);
      bufferedString = buf;
    } else {
      bufferedString += data;
    }
  });
  
  port.on('close', function (data) {
    console.log('close: ' + data);
    document.getElementById("connectButton").innerHTML = "Connect"
    connectedPort = "";
    fileName = "";
  });

  port.on('disconnect', function (data) {
    console.log('disconnect: ' + data);
    document.getElementById("connectButton").innerHTML = "Connect"
    connectedPort = "";
    fileName = "";
  });
}

function disconnect() {
  port.close();
}

function send(message) {
  port.write(message, function(err) {
      if (err) {
        console.log('Error on write: ', err.message);
        return;
      }
      console.log('message written');
    });
}