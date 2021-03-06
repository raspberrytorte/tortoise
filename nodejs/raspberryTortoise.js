#!/usr/bin/env node

var http = require('http');
var sys = require('util');
var url = require("url");
var childProcessExec = require('child_process').exec;
var requests = [];
var querystring = require("querystring");

var requestHandlers = {}
requestHandlers["/"] = showForm;
requestHandlers["/index"] = showForm;
//requestHandlers["/execute"] = executeCommand;
requestHandlers["/left"] = executeLeft;
requestHandlers["/right"] = executeRight;
requestHandlers["/forward"] = executeForward;
requestHandlers["/backward"] = executeBackward;

// i think this is assuming launching using
// node raspberryTortoise.js, rather than
// ./raspberryTortoise.js
var listenAddress = process.argv[2];
if (undefined == listenAddress)
{
    listenAddress = "192.168.0.10";
}
var listenPort = process.argv[3];
if (undefined == listenPort)
{
    listenPort = 8080;
}

// Path to the location of the main drive
var drivePath = process.argv[4];
if (undefined == drivePath)
{
    drivePath = "../drive/";
}


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

//function executeCommand(serverRequest, serverResponse, incominString)
function executeCommand(serverRequest, serverResponse, postedData)
{
  //  var incomingString = url.parse(postedData).text;
    var num = url.parse(serverRequest.url).query ;
    var pathname = url.parse(serverRequest.url).pathname;


    var incomingString = ".././drive --" + pathname + " num" +" &";

//    var incomingString = "ls -" +num ;

    console.log("command found " + incomingString);

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
                serverResponse.end();
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
    var body = '<html><head><title>Raspberry Tortoise Controller</title>' +
               '<meta name="viewport" content="width=340"> ' +
               '</head><body><div id="main"> ' +
               '<font face="Verdana"> ' +
               '<h1>Raspberry Tortoise</h1>' +
               '<p> The stream below is mjpeg @2fps from the ' +
               '<a href="http://raspberrytorte.com/index.php?title=RaspberryTortoise"> ' + 
               ' RaspberryTortoise </a> </p>' +
               '<p> Use the link below to control the Tortoise </p> ' +
               ' <p> ' +
               '   <a href="./left?0.5"> |Left</a>' +
               '   <a href="./right?0.5"> |Right</a>' + 
               '   <a href="./forward?0.05"> |Forward</a>' + 
               '   <a href="./backward?0.05"> |Backward|</a>' + 
               ' </p>  <img src="http://' + listenAddress + ':8081/"/> ' +
               '<p>Don`t see the video? Use Firefox or Chrome browsers.</p>  ' +
               '</font>' +
               '</body></html>';

    serverResponse.writeHead(200, {"Content-Type": "text/html"});
    serverResponse.write(body);
    serverResponse.end();
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


function executeLeft(serverRequest, serverResponse, postedData)
{
    var num = url.parse(serverRequest.url).query ;
    var incomingString = drivePath + "./drive --left " + num + " &";
    console.log("command found " + incomingString);
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
        console.log("command found " + responseString);
       showForm(serverRequest, serverResponse, postedData)
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(incomingString, options, systemCommandResponseCallback);
}

function executeRight(serverRequest, serverResponse, postedData)
{
    var num = url.parse(serverRequest.url).query ;
    var incomingString = drivePath + "./drive --right " +  num + " &";
    console.log("command found " + incomingString);
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
        console.log("command found " + responseString);
       showForm(serverRequest, serverResponse, postedData)
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(incomingString, options, systemCommandResponseCallback);
}


function executeForward(serverRequest, serverResponse, postedData)
{
    var num = url.parse(serverRequest.url).query ;
    var incomingString = drivePath + "./drive --forward " +  num + " &";
    console.log("command found " + incomingString);
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
        console.log("command found " + responseString);
       showForm(serverRequest, serverResponse, postedData)
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(incomingString, options, systemCommandResponseCallback);
}

function executeBackward(serverRequest, serverResponse, postedData)
{
    var num = url.parse(serverRequest.url).query ;
    var incomingString = drivePath + "./drive --backward " +  num + " &";
    console.log("command found " + incomingString);
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
        console.log("command found " + responseString);
       showForm(serverRequest, serverResponse, postedData)
    }

    // Reminder 'child_process.exec' takes 3 arguments: command, [options], callback
    var child = childProcessExec(incomingString, options, systemCommandResponseCallback);
}
