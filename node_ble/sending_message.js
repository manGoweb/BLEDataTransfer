const packetLen = require('./config').packetLength;
const startSymbol = require('./config').startingSymbol;

const md5 = require('md5');

function toBytesInt32 (num) {
    arr = new Uint8Array([
        (num & 0x000000ff),
        (num & 0x0000ff00) >> 8,
        (num & 0x00ff0000) >> 16,
        (num & 0xff000000) >> 24
    ]);
    return arr;
}

module.exports = class SendingMessage {

    constructor(message) {
        this.message = message;

        this.firstPacket = this.createFirstPacket();
        console.log("first p: ", this.firstPacket);
        this.secondPacket = this.createSecondPacket();
        console.log("second p: ",this.secondPacket);
    }

    createFirstPacket() {

        let arr = [];
        let first8B = [];
        // Start sequence
        for(let i = 0; i < 4; i++) {
            arr[i] = startSymbol;
            first8B[i] = startSymbol;
        }

        // 4 message length
        let messageLenBytes = toBytesInt32(this.message.length);
        for(let i = 4; i < 8; i++) {
            arr[i] = messageLenBytes[i-4];
            if(messageLenBytes[i-4] == 0x00) {
                first8B[i] = 0x30;
            }
            else {
                first8B[i] = messageLenBytes[i-4];
            }
        }

        let md5hash = md5(new Buffer(first8B));
        for(let i = 8; i < packetLen; i++) {
            arr[i] = md5hash.charCodeAt(i-8);
        }
        console.log("HASH:")
        console.log(md5hash);
        console.log(new Buffer(arr));
        return new Buffer(arr);

    }

    createSecondPacket() {

        let md5hash = md5(this.message);
        let md5hashCutted = md5hash.substring(0,packetLen);
        let hashCharArr = md5hashCutted.split('').map((c) => {
            return c.charCodeAt(0);
        });

        return new Buffer(hashCharArr);

    }

    send(characteristic, callback) {

        // FIRST PACKET
        characteristic.write(this.firstPacket, true, callback);

        // SECOND PACKET
        characteristic.write(this.secondPacket, true, callback);

        // DATA
        let msgCount = Math.ceil(this.message.length / packetLen);
        for(let i = 0; i < msgCount; i++) {
            let packetData = this.message.substring(i*packetLen,(i*packetLen)+packetLen);
            characteristic.write(new Buffer(packetData), true, callback);
        }

        console.log("Whole message send");

    }

};