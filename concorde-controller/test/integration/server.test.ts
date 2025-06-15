import test from 'node:test';
import assert from 'node:assert/strict';
import { once } from 'events';
import { WebSocket } from 'ws';
import path from 'path';
import { fileURLToPath } from 'url';
import { startServer } from '../../src/server.js';

const __dirname = path.dirname(fileURLToPath(import.meta.url));

function waitMessage(ws: WebSocket) {
  return once(ws, 'message').then(([data]) => JSON.parse(data.toString()));
}

test('basic websocket workflow', async () => {
  process.env.KEY_PATH = path.join(__dirname, '../fixtures/test-key.pem');
  process.env.CERT_PATH = path.join(__dirname, '../fixtures/test-cert.pem');
  process.env.PORT = '0';

  const { httpServer, wss } = startServer();
  const port = (httpServer.address() as any).port;
  const url = `wss://localhost:${port}`;

  const servo = new WebSocket(url, { rejectUnauthorized: false });
  await once(servo, 'open');
  servo.send(JSON.stringify({ event: 'register', id: 'SERVO_1' }));
  await waitMessage(servo); // register success

  const nfc = new WebSocket(url, { rejectUnauthorized: false });
  await once(nfc, 'open');
  nfc.send(JSON.stringify({ event: 'register', id: 'NFC_1' }));
  await waitMessage(nfc); // register success

  nfc.send(JSON.stringify({ event: 'nfc', id: 'NFC_1', uuid: '04:ac:09:6a:06:1f:94' }));
  const servoMsg = await waitMessage(servo);
  assert.equal(servoMsg.event, 'start_spin');

  servo.close();
  nfc.close();
  await Promise.all([once(servo, 'close'), once(nfc, 'close')]);
  wss.close();
  httpServer.close();
});
