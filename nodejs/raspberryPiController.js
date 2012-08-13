
var http = require('http');
var sys = require('sys');
var url = require("url");
var childProcessExec = require('child_process').exec;
var requests = [];
var querystring = require("querystring");

var requestHandlers = {}
requestHandlers["/"] = showForm;
requestHandlers["/index"] = showForm;
requestHandlers["/execute"] = executeForm;

const listenAddress = "127.0.0.1";
const listenPort = 8124;
http.createServer(requestListener).listen(listenPort, listenAddress);
console.log('Server running at http://' + listenAddress + ':' + listenPort);

// if the stream output from the command is non-empty it ends in a newline.
// this function simply makes the formatting consistent in the empty case
function pipeToString(pipe)
{
    if (pipe === "")
    {
        return '\n';
    }
    else
    {
        return pipe;
    }
}

function executeForm(serverRequest, serverResponse, postedData)
{
    var incomingString = querystring.parse(postedData).text;

    var options = {
        encoding: 'utf8',
        timeout: 0,
        maxBuffer: 200 * 1024,
        killSignal: 'SIGTERM',
        cwd: null,
        env: null
    }

    // Create a closure over the callback so it has access to the request to respond to
    function systemCommandResponseCallback(error, stdout, stderr)
    {
        var responseString =
            'stdout: ' + pipeToString(stdout)
            + 'stderr: ' + pipeToString(stderr)
            + 'error: ' + error;

    	// Write the response and close the request
		serverResponse.writeHead(200, { "Content-Type": "text/plain" });
		serverResponse.end("RPi RESPONSE:\n" + responseString);
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(incomingString, options, systemCommandResponseCallback);
}

setInterval(function() {
	// close out requests older than 30 seconds
    const timeoutMilliseconds = 30000;
	var expiration = new Date().getTime() - timeoutMilliseconds;
	var response;

	for (var i = requests.length - 1; i >= 0; i--)
	{
		if (requests[i].timestamp < expiration)
		{
		    // Send a dummy response such that if the operation takes a long time, we don't time-out..
			response = requests[i].response;
			response.writeHead(200, { "Content-Type": "text/plain" });
			response.end("RPi request didn't complete in " + timeoutMilliseconds + "ms");
		}
	}
}, 1000);

function showForm(serverRequest, serverResponse, postedData)
{
    console.log("Showing form...");

    // Generate a simple form tho allow the client to submit a string
    var body = '<html>'+
        '<head>'+
        '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />'+
        '</head>'+
        '<body>'+
        '<form action="/execute" method="post">'+
        '<textarea name="text" rows="20" cols="60"></textarea>'+
        '<input type="submit" value="Submit text" />'+
        '</form>'+
        '</body>'+
        '</html>';

    serverResponse.writeHead(200, {"Content-Type": "text/html"});
    serverResponse.write(body);
    serverResponse.end();
    console.log("Form shown.");
}

function router(serverRequest, serverResponse, postedData)
{
    var pathname = url.parse(serverRequest.url).pathname;
    console.log("Request for " + pathname + " received..");
    if (typeof requestHandlers[pathname] === 'function')
    {
        requestHandlers[pathname](serverRequest, serverResponse, postedData);
    }
    else
    {
        console.log("No handler for " + pathname);
    }
}

// The request listener is added to the request event
// Simply record the request for future response, then kick off an asynchonous call to the system
function requestListener(serverRequest, serverResponse)
{
    var postedData = "";

    console.log("Connection received..");

    serverRequest.setEncoding("utf8");

    // as we receive data append it to a variable within this closure
    serverRequest.addListener("data", function(postDataChunk) {
        postedData += postDataChunk;
        console.log("Received POST data chunk '"+ postDataChunk + "'.");
    });

    // when an end is received all the posted data has been collected, the request is ready to be handled
    serverRequest.addListener("end", function() {
        router(serverRequest, serverResponse, postedData);
    });
}


