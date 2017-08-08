## Fast Rabbit
High Throughput processing Rabbit MQ messages in c++ native add on using N-API prototype

##configuration
Make sure you have node 8+ installed with node-gyp


    $npm install


    $node-gyp configure


    $node-gyp rebuild



##To start the listener 

    $node --napi-modules msg_process.js

##To push messages

    $node msg_produce.js


