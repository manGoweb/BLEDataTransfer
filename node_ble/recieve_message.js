const _ = require('underscore');
const startSymbol = require('./config').startingSymbol;
const md5 = require('md5');

const packetLen = require('./config').packetLength;

module.exports = class RecieveMessage {
    constructor(firstPacket, secondPacket) {
        // FIRST PACKET

        // 4B of starting sequence
        if(!RecieveMessage.isStarterPacket(firstPacket)) {
            throw new Error("Wrong constructor parameter, must be a first packet");
        }

        // 4B of message length
        this.messageLen = firstPacket.readUInt32LE(4,8);

        // 12B of first 12B of md5 of previous 8B
        let recMd5 = '';
        for(let i = 8; i < packetLen; i++) {
            recMd5 += String.fromCharCode(firstPacket[i]);
        }

        let first8B = [];
        for(let i = 0; i<8; i++) {
            if(firstPacket[i] == 0x00) {
                first8B[i] = 0x30;
            } else {
                first8B[i] = firstPacket[i];
            }
        }

        let computedMd5 = md5(new Buffer(first8B));
        if(!RecieveMessage.compareHashes(recMd5,computedMd5,12)) {
            throw new Error("Invalid message hash!");
        }


        // SECOND PACKET
        // 20B of following message md5
        this.receivedMessageMd5 = '';
        for(let i = 0; i < packetLen; i++) {
            this.receivedMessageMd5 += String.fromCharCode(secondPacket[i]);
        }
        console.log("recieved msg md5: ", this.receivedMessageMd5);

    }

    addPacket(msg) {
        if(RecieveMessage.isStarterPacket(msg)) {
            throw new Error("Starting packet given!");
        }

        // add recieved string
        if(!this.msg) {
            this.msg = '';
        }
        for(let i = 0; i < packetLen; i++) {
            if(msg[i] !== undefined) {
                this.msg += String.fromCharCode(msg[i]);
            }
        }

        if(this.msg.length > this.messageLen) {
            throw new Error("Invalid message length!");
        }

        // on complete, this could be remade to nonblocking emmiting of an event
        if(this.msg.length == this.messageLen) {
            if(this.isValid()) {
                console.log("message complete: " +this.msg);
                this.messageComplete = true;
            }
        }
    }

    /**
     * Gives true when it is message starting packet (4 starting bytes plus md5 hash)
     * @param msg
     */
    static isStarterPacket(buffer) {
        if(buffer.length !== packetLen)
            return false;

        for(let i = 0; i < 4; i++) {
            if(buffer[i] !== startSymbol)
                return false;
        }

        return true;
    }

    /**
     * Compares first @count characters of given hashes
     * @param hash1
     * @param hash2
     * @param count
     */
    static compareHashes(hash1, hash2, count) {

        for(let i = 0; i < count; i++) {
            if(hash1.charAt(i) !== hash2.charAt(i)) {
                return false;
            }
        }

        return true;

    }

    /**
     * Counts md5 of current message and compares it with received md5
     */
    isValid() {

        return RecieveMessage.compareHashes(this.receivedMessageMd5, md5(this.msg), this.receivedMessageMd5.length);

    }
};