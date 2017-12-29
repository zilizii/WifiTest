# WifiTest
ESP8266 with WebServer and WebSocket

Basic example how to use a Websocket and WebServer on ESP8266 boards. This practicular example used to run on a ESP-01 board.

The clients connected to the webserver and the downloads the webpage which contains the Websocket script as well to make connection bact to the server.
If the state changes by any connected users the server sent the notifications to the other connected clients via Websocket. In this case the page no needs to be reload.
