
var registeredReceivers = {};
var STATE_DISCONNECTED = 0;
var STATE_CONNECTED = 1;
var state = STATE_DISCONNECTED;

console.log("Initializing Duplex API");

registeredReceivers.TEMP = function(cmd, params){
	if (cmd=="CT") {			// curren temp
		document.getElementById("tempValueHC").innerText = parseInt(params[0]);
		document.getElementById("tempValueW").innerText = parseInt(params[1]);
	} else {
		console.error("Unknown command: " + cmd);
	}
};

registeredReceivers.IOC = function(cmd, params){
	if (cmd=="IOST") {			// io state
		document.getElementById("workingHC").style.visibility = params[0]=="1" ? "visible" : "hidden";
		document.getElementById("workingW").style.visibility = params[1]=="1" ? "visible" : "hidden";
		document.getElementById("waterFlow").style.visibility = params[2]=="1" ? "visible" : "hidden";
	} else {
		console.error("Unknown command: " + cmd);
	}
};

registeredReceivers.BT = function(cmd, params) {
	if (cmd=="LOG") {
		console.log("> LogMsg(" + params[0] + "): " + params[1].replace("_", " "));
	} else {
		console.error("Unknown command: " + cmd);
	}
}

registeredReceivers.TIME = function(cmd, params){
	if (cmd=="CT") {		// current time
		document.getElementById("timeValue").innerText = formatTime(params[0]);
	} else {
		console.error("Unknown command: " + cmd);
	}
};

registeredReceivers.SETT = function(cmd, params) {
	if (cmd=="CS") {			// current settings
		var values = params[0].split("&");
		$( "#sliderOperatingTempHC" ).slider('value', parseInt(values[0]));
		$( "#sliderOperatingTempW" ).slider('value', parseInt(values[1]));
		//parseInt(values[2])
		//parseInt(values[3])
		$( "#sliderPreheatingTempHC" ).slider('value', parseInt(values[4]));
		$( "#sliderPreheatingTempW" ).slider('value', parseInt(values[5]));
		//parseInt(values[6])
	} else {
		console.error("Unknown command: " + cmd);
	}
};

function formatTime(timeSec) {
	var hours = Math.floor(timeSec/(24*60));
	timeSec-=hours*24*60;
	var minutes = Math.floor(timeSec/60);
	timeSec-=minutes*60;
	return _formatTimePart(hours) + ":" + _formatTimePart(minutes) + ":" + _formatTimePart(timeSec);
}

function _formatTimePart(part) {
	part = part + "";
	if (part.length==1) part="0" + part;
	return part;
}

function onDataReceived(receiver, cmd) {
	console.log("OnDataReceived: " + receiver + ", " + cmd);
	
	var params = cmd.split(" ");
	var cmd = params[0];
	params.splice(0, 1);

	if (registeredReceivers[receiver]) {
		registeredReceivers[receiver](cmd, params);
	} else {
		console.error("Unknown receiver: " + receiver);
	}
}

function onStateChanged(newState) {
	state = newState;
	console.log("State changed to " + state);
	
	if (state==STATE_CONNECTED) {
		sendData("TIME", "GET");
		sendData("TEMP", "GET");
		sendData("SETT", "GET");
		sendData("IOST", "GET");
	}
}

function sendData(receiver, cmd) {
	console.log("sendData: " + receiver + " " + cmd);
	if (dataApi) {
		dataApi.sendData(receiver, cmd);
	} else {
		console.error("dataApi object is null !");
	}
}