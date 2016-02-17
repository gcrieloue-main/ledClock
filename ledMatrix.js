var net = require('net');
var client = net.connect({port:5001},
        function()
        {
            console.log('Connected to server');
        });

function send(command)
{
    console.log(command);
    client.write(command);
}

module.exports.send=send;

