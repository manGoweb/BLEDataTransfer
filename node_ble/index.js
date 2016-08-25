const noble = require('noble');
const _ = require('underscore');
const randomstring = require('randomstring');

const redbearUuid = require('./config').redbearUuid;
const messageReadCharUuid = require('./config').messageReadCharUuid;
const messageWriteCharUuid = require('./config').messageWriteCharUuid;

const RecievedMessage = require('./recieve_message');
const SendingMessage = require('./sending_message');

noble.on('stateChange', function(state) {
    if (state === 'poweredOn') {
        console.log('scanning...');
        noble.startScanning([redbearUuid], false);
    }
    else {
        console.log("stop scanning");
        noble.stopScanning();
    }
});

let messageReadCharacteristic = null;
let messageWriteCharacteristic = null;

noble.on('discover', function(peripheral) {
    // we found a peripheral
    console.log("found peripheral", peripheral.advertisement);
    noble.stopScanning();

    peripheral.connect((err) => {

        console.log("Connected");

        peripheral.discoverServices([], (err, services) => {

            services.forEach((service) => {

                console.log('found service:', service.uuid);
                service.discoverCharacteristics([], (err, characteristics) => {

                    characteristics.forEach((characteristic) => {

                        console.log("found characteristic:", characteristic.uuid);

                        if(messageReadCharUuid == characteristic.uuid) {
                            console.log("got message read characteristic");
                            messageReadCharacteristic = characteristic;
                        }
                        else if(messageWriteCharUuid == characteristic.uuid) {
                            console.log("got message write characteristic");
                            messageWriteCharacteristic = characteristic;
                        }

                    });

                    if(!messageReadCharacteristic || !messageWriteCharacteristic) {
                        throw new Error("Characteristics setup failed");
                    }

                    messagesSubscribe();

                });

            });

        });

    });


});

let messages = [];

function messagesSubscribe() {

    let curMessage = null;
    let starterPacket = null;
    messageReadCharacteristic.on('read', (data) => {


        if(RecievedMessage.isStarterPacket(data)) {
            if(curMessage) {
                messages.push(curMessage);
            }
            starterPacket = data;
        }
        else if(starterPacket) {
            curMessage = new RecievedMessage(starterPacket, data);
            starterPacket = null;
        }
        else {
            if(curMessage) {
                curMessage.addPacket(data);
            }
        }


    });

    messageReadCharacteristic.subscribe((err) => {

        console.log("SUBSCRIBE", err);

        setInterval(sendMessage, 2000);

    });



}

function sendMessage() {

    let message = randomstring.generate((Math.random() * 1000) + 1);
    //message = "Hey whats upp? This is rly long message including json object: {\"propertyName\": \"propertyValue\"} ";
    console.log(`Sending a message(${message.length}): ${message}`);


    let messageToSend = new SendingMessage(message);
    messageToSend.send(messageWriteCharacteristic, (err) => {
        if(err) throw err;
    });

}

