const net = require("net");

const client = net.createConnection({host: "16777343", port: 8767}, () => {

    console.log("connected");

    let buffer = Buffer.alloc(256);
    buffer.write("Test Data From Node.js");

    client.write(buffer.toString());
});

client.on("error", (error) => {
    console.log(error.stack);
})