var amqp = require('amqplib/callback_api');
var addon = require('bindings')('addon');
//const callexec = require('bindings')('callexec');

process.env.UV_THREADPOOL_SIZE = 100
var theBls = new addon.BlsApi(10);
amqp.connect('amqp://localhost', function(err, conn) {
    var channels = new Array();
    //for (let i = 0; i < 2; i++) 
    //{

    conn.createChannel(function(err, ch) {
        var q = 'looger';
        ch.prefetch(100);
        var i = 0;
        ch.assertQueue(q, { durable: true });
        console.log(" [*] Waiting for messages in %s. To exit press CTRL+C", q);
        ch.consume(q, function(msg) {
            i = i++;
            console.log('recieved ' + i);
            var sec = 1;

            // console.log(" %s Received %s", ch.id, msg.content.toString());
            // setTimeout(function() {

            // theBls.plusOne();
            theBls.multiply(i);
            ch.ack(msg);
            console.log(" logger ", msg.content.toString());
            //  }, 1000);

        }, { noAck: false });

        channels.push(ch);
    });
    conn.createChannel(function(err, ch) {
        var q = 'newway';
        ch.prefetch(1000);
        var i = 0;
        ch.assertQueue(q, { durable: true });
        console.log(" [*] Waiting for messages in %s. To exit press CTRL+C", q);
        ch.consume(q, function(msg) {
            i = i++;
            console.log('recieved ' + msg.content);
            // theBls.delay(parseInt(msg.content));
            // callexec.async_bls(function(err, arg) {
            //             console.log(arg);
            //             ch.ack(msg);
            //         }

            //     )
            //  ch.ack(msg);
            //  console.log(" Done ", msg.content.toString());
            //  }, 1000);
        }, { noAck: false });

        channels.push(ch);
    });
    // }
});