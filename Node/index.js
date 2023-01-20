const { SerialPort } = require('serialport')
const { DelimiterParser } = require('@serialport/parser-delimiter')

const serialPath = "COM4"
const serialport = new SerialPort({ path: serialPath, baudRate: 115200 })

serialport.on('open', () => {
    console.log("OPEN");
    // serialport.write('HELLO\n')
})

const parser = serialport.pipe(new DelimiterParser({ delimiter: '\n' }))
parser.on('data', (data) => {
    console.log("Receiving data: " + data);
});

const express = require('express');
const app = express();
const PORT = 3000;


app.use(express.static('public'));
app.use(express.json());

app.post('/', function (request, response) {
    const data = request.body;

    let text = data.text;
    console.log("Sending data: " + text);
    if (serialport.isOpen) {
        serialport.write(text + '\n');
    }

    response.send(request.body);
});

app.listen(PORT, () => console.log(`Server listening on port: ${PORT}`));