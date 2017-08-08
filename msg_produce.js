#!/usr/bin/env node

var amqp = require('amqplib/callback_api');

amqp.connect('amqp://localhost', function(err, conn) {
    conn.createChannel(function(err, ch) {
        var q = 'OCWorkque';
        var msg = process.argv.slice(2).join(' ') || "Hello lean and mean";


        ch.assertQueue(q, { durable: true });
        for (let i = 0; i < 2; i++) {
            {
                ch.sendToQueue(q, new Buffer(i.toString()), { persistent: true });
                console.log(" [x] Sent '%s'", msg + i.toString());
            }
        }
    });
    setTimeout(function() {
        conn.close();
        process.exit(0)
    }, 500);
});