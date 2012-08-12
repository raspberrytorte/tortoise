
var http = require('http');
var sys = require('sys');
var childProcessExec = require('child_process').exec;
var requests = [];

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

function executeSystemCommand(commandString, thisRequestIndex)
{
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

    	var response = requests[thisRequestIndex].response;
    	// Write the response and close the request
		response.writeHead(200, { "Content-Type": "text/plain" });
		response.end("RPi RESPONSE:\n" + responseString);
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(commandString, options, systemCommandResponseCallback);
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

// The request listener is added to the request event
// Simply record the request for future response, then kick off an asynchonous call to the system
function requestListener(serverRequest, serverResponse)
{
    var nextRequestIndex = requests.length;

	// store the response so we can respond later
	requests.push({
		response: serverResponse,
		timestamp: new Date().getTime()
	});

	console.log('Received request ' + nextRequestIndex + ' @ ' + requests[nextRequestIndex].timestamp);

	executeSystemCommand('python -V', nextRequestIndex);
}

