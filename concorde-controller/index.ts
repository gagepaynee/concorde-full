import { WebSocketServer, WebSocket } from 'ws';
import { createServer } from 'https';
import fs from 'fs';
import dotenv from 'dotenv';

dotenv.config()

const options = {
  key: fs.readFileSync(process.env.KEY_PATH as string),
  cert: fs.readFileSync(process.env.CERT_PATH as string),
};

// Respond to HTTP(S) requests too
const server = createServer(options, (req, res) => {
  res.writeHead(200);
  res.end('WebSocket server is running');
});

const port = Number(process.env.PORT || 0);
server.listen(port, '0.0.0.0', () => {
  console.log(`Server listening on PORT: ${port}` );
});

interface ExtendedWebSocket extends WebSocket {
  uid?: string;
  role?: string;
}

const wss = new WebSocketServer({ server });

function registerSocket(socket: ExtendedWebSocket, id: string, event: string): void {
  console.log(`${event}: `, id);
  socket.uid = id;
  socket.role = event;
  const message = JSON.stringify({ event: `${event}_success`, id });

  // Notify all setup sockets when a client registers successfully
  if (event === 'register') {
    wss.clients.forEach((client) => {
      const wsClient = client as ExtendedWebSocket;
      if (wsClient.role === 'setup') {
        console.log('sending');
        wsClient.send(message);
      }
    });
  }

  socket.send(message);
}

wss.on('connection', (socket: ExtendedWebSocket) => {
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
          const wsClient = client as ExtendedWebSocket;
          if (wsClient.uid === data.id) {
            wsClient.send(JSON.stringify({ event: 'unlocked', id: data.id }));
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
