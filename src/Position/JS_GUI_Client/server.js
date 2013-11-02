var net = require("net");
var io = require("socket.io").listen(3001);

io.set('log level', 1);
io.sockets.on('connection', function(websocket) {
    var tcpSocket = new net.Socket();
    tcpSocket.setEncoding('utf8');
    websocket.on('put', function() {
        tcpSocket.write('get\n');
    });
    
    tcpSocket.connect(3000, function() {
        var tempData = "";
        tcpSocket.on('data', function(data) {
            if (data.search("end") > 0) {
                var data1 = tempData + data;
                console.log("data: " + data1);
                var s = data1.split("\n");
                websocket.emit('points', s);
                tempData = "";
            } else {
                tempData = data;
            }
        });
    });

});

