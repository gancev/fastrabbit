# Fast Rabbit
High Throughput processing Rabbit MQ messages in c++ native add on using N-API
#configuration
Make sure you have node 8+ installed with node-gyp
1. npm install
2. node-gyp configure
3. node-gyp rebuild

#To start the listener 
node --napi-modules msg_process.js

#To push messages
node msg_produce.js


