import { createServer } from 'https';
import { WebSocketServer } from 'ws';
import fs from 'fs';
import dotenv from 'dotenv';
import { ClientManager, ManagedWebSocket } from './clients.js';

dotenv.config();

export function startServer() {
  const options = {
    key: fs.readFileSync(process.env.KEY_PATH as string),
    cert: fs.readFileSync(process.env.CERT_PATH as string),
  };

  const httpServer = createServer(options, (_req, res) => {
    res.writeHead(200);
    res.end('WebSocket server is running');
  });

  const port = Number(process.env.PORT || 0);
  httpServer.listen(port, '0.0.0.0', () => {
    console.log(`Server listening on PORT: ${port}`);
  });

  const wss = new WebSocketServer({ server: httpServer });
  const clients = new ClientManager();
  let servoSpinning = false;

  wss.on('connection', (socket: ManagedWebSocket) => {
    socket.on('message', (msg) => {
      let data: any;
      try {
        data = JSON.parse(msg.toString());
      } catch (err) {
        console.error('Invalid JSON:', err);
        return;
      }

      switch (data.event) {
        case 'register':
          clients.register(data.id, 'register', socket);
          socket.send(JSON.stringify({ event: 'register_success', id: data.id }));
          clients.broadcast('setup', JSON.stringify({ event: 'register_success', id: data.id }));
          break;
        case 'setup':
          clients.register(data.id, 'setup', socket);
          socket.send(JSON.stringify({ event: 'setup_success', id: data.id }));
          break;
        case 'unlocked':
          console.log('unlock: ', data.id);
          clients.send('register', data.id, JSON.stringify({ event: 'unlocked', id: data.id }));
          break;
        case 'nfc':
          console.log(`Reader with id ${data.id} just read: ${data.uuid}`);
          if (data.id === 'NFC_1' && data.uuid === '04:ac:09:6a:06:1f:94') {
            servoSpinning = !servoSpinning;
            const event = servoSpinning ? 'start_spin' : 'stop_spin';
            clients.send('register', 'SERVO_1', JSON.stringify({ event, id: 'SERVO_1' }));
          }
          break;
        default:
          console.log('Received event:', data.event);
          break;
      }
    });

    socket.on('close', () => {
      clients.unregister(socket);
    });
  });

  return { httpServer, wss, clients };
}
