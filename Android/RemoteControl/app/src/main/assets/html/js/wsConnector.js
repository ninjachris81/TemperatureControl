
var ws;

function initWsConnector(url) {
	console.log("Initializing WS Connector");
	
	ws = new ReconnectingWebSocket(url, ["tc"]);
	ws.reconnectInterval = 1000;
	ws.timeoutInterval = 20000;
	ws.debug = true;
	
	ws.onopen = function () {
		console.log("Connected");
		//connection.send('Ping');
	};

	// Log errors
	ws.onerror = function (error) {
	  console.log('WebSocket Error ' + error);
	};

	// Log messages from the server
	ws.onmessage = function (e) {
	  console.log('Server: ' + e.data);
	};
}