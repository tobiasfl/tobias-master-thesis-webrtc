'use strict'

/**
 * Created by safiqul on 07.10.15.
 */
var nodeStatic = require('node-static');
var http = require('http');
//var fs = require('fs');
//var options = {
//    key: fs.readFileSync('key.pem'),
//    cert: fs.readFileSync('cert.pem')
//};

// Create a node-static server instance
var file = new(nodeStatic.Server)();
// We use the http module’s createServer function and
// rely on our instance of node-static to serve the files
var app = http.createServer(function (req, res) {
    file.serve(req, res);
}).listen(51002);
console.log("Listening on port 51002");
// Use socket.io JavaScript library for real-time web applications
var io = require('socket.io').listen(app);

// Let's start managing connections...
io.sockets.on('connection', function (socket){
    socket.on('message', function (message, room) {
        log('got message: ' + message + ' to room ' + room);
        socket.broadcast.to(room).emit('message', message);
    });
    console.log("new connection");
    // Handle 'create or join' messages
    socket.on('join_room', function (room) {
        var numClients = io.engine.clientsCount;
        console.log("client joining a room");
        if (numClients == 1) {
            console.log("creating room");
            socket.join(room);
            socket.emit('created', room);
            log("Created room " + room)
        } else if (numClients == 2) {
            console.log("2nd client joined");
            io.sockets.in(room).emit('join', room, numClients);
            socket.join(room);
            socket.emit('joined', room);
            log("Joined room " + room)
        } else {
            socket.emit('full', room);
            log("Rejected client: room" + room + "full");
        }
    });
    function log(){
        var array = ["SERVER: "];
        for (var i = 0; i < arguments.length; i++) {
            array.push(arguments[i]);
        }
        socket.emit('log', array);
    }
});
