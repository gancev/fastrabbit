var callexec = require('./build/Release/msgprocessor');
var amqp = require('amqplib/callback_api');
callexec.async_bls("{next:2,arr:[1,2,3]}", function(err, arg) {
    console.log('callback  ' + arg);

});

amqp.connect('amqp://localhost', function(err, conn) {
    conn.createChannel(function(err, ch) {
        var q = 'OCWorkque';
        ch.prefetch(1000);
        ch.assertQueue(q, { durable: true });
        console.log(" [*] Waiting for messages in %s. To exit press CTRL+C", q);
        ch.consume(q, function(msg) {

            callexec.async_bls(JSON.stringify(msg), function(err, arg) {
                console.log('processed  ' + arg);
                ch.ack(msg);
            });


        }, { noAck: false });

    });
    // }
});