var callexec = require('./build/Release/callexec');
var amqp = require('amqplib/callback_api');


amqp.connect('amqp://localhost', function(err, conn) {

    conn.createChannel(function(err, ch) {
        var q = 'newway';
        ch.prefetch(1000);
        var i = 0;
        ch.assertQueue(q, { durable: true });
        console.log(" [*] Waiting for messages in %s. To exit press CTRL+C", q);
        ch.consume(q, function(msg) {
            i = i++;
            console.log('recieved ' + msg.content);
            callexec.async_bls(function(err, arg) {
                console.log('callback  ' + msg.content + arg);
                ch.ack(msg);
            });


        }, { noAck: false });

    });
    // }
});

callexec.async_bls(function(err, arg) {
    console.log(arg)
});