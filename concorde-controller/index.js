import { WebSocketServer } from 'ws';
import { createServer } from 'https';
import fs from 'fs';
import dotenv from 'dotenv';

dotenv.config()

const options = {
  key: fs.readFileSync(process.env.KEY_PATH),
  cert: fs.readFileSync(process.env.CERT_PATH),
};

// Respond to HTTP(S) requests too
const server = createServer(options, (req, res) => {
  res.writeHead(200);
  res.end('WebSocket server is running');
});

server.listen(process.env.PORT, '0.0.0.0', () => {
  console.log(`Server listening on PORT: ${process.env.PORT}` );
});

const wss = new WebSocketServer({ server });

function registerSocket(socket, id, event) {
  console.log(`${event}: `, id);
  socket.uid = id;
  socket.role = event;
  const message = JSON.stringify({ event: `${event}_success`, id });

  // Notify all setup sockets when a client registers successfully
  if (event === 'register') {
    wss.clients.forEach((client) => {
      if (client.role === 'setup') {
        console.log('sending');
        client.send(message);
      }
    });
  }

  socket.send(message);
}

wss.on('connection', (socket) => {
  socket.on('message', (message) => {
    let data;
    try {
      data = JSON.parse(message.toString());
    } catch (err) {
      console.error('Invalid JSON:', err);
      return;
    }

      switch (data.event) {
        case 'register':
          registerSocket(socket, data.id, 'register');
          break;

        case 'setup':
          registerSocket(socket, data.id, 'setup');
          break;

      case 'unlocked':
        console.log('unlock: ', data.id);
        wss.clients.forEach((client) => {
          if (client.uid === data.id) {
            client.send(JSON.stringify({ event: 'unlocked', id: data.id }));
          }
        });
        break;

      case 'nfc':
        console.log(`Reader with id ${data.id} just read: ${data.uuid}`);
        break;

      default:
        console.log('Received event:', data.event);
        break;
    }
  });
});
