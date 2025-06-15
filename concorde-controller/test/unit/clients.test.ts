import test from 'node:test';
import assert from 'node:assert/strict';
import { ClientManager, ManagedWebSocket } from '../../src/clients.js';

test('register and unregister clients', () => {
  const mgr = new ClientManager();
  const sock: any = { send: () => {} } as ManagedWebSocket;
  mgr.register('c1', 'role', sock);
  assert.equal(sock.id, 'c1');
  assert.equal(sock.role, 'role');
  (mgr as any).clients.get('role').has('c1');
  mgr.unregister(sock);
  assert.ok(!(mgr as any).clients.get('role')?.has('c1'));
});

test('send to single client', () => {
  const mgr = new ClientManager();
  let msg = '';
  const sock: any = { send: (m: string) => { msg = m; } } as ManagedWebSocket;
  mgr.register('c1', 'role', sock);
  mgr.send('role', 'c1', 'hello');
  assert.equal(msg, 'hello');
});

test('broadcast to all role clients', () => {
  const mgr = new ClientManager();
  const msgs: string[] = [];
  const mkSock = () => ({ send: (m: string) => msgs.push(m) }) as ManagedWebSocket;
  mgr.register('a', 'role', mkSock());
  mgr.register('b', 'role', mkSock());
  mgr.broadcast('role', 'hi');
  assert.equal(msgs.length, 2);
  assert.deepEqual(msgs, ['hi', 'hi']);
});
