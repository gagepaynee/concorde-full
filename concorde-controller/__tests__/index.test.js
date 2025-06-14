import { registerSocket } from '../index.js';

describe('registerSocket', () => {
  test('register event sets properties and notifies setup sockets', () => {
    const socket = { send: jest.fn() };
    const setup1 = { role: 'setup', send: jest.fn() };
    const setup2 = { role: 'setup', send: jest.fn() };
    const wss = { clients: new Set([setup1, setup2]) };

    registerSocket(socket, 'abc', 'register', wss);

    const msg = JSON.stringify({ event: 'register_success', id: 'abc' });
    expect(socket.uid).toBe('abc');
    expect(socket.role).toBe('register');
    expect(socket.send).toHaveBeenCalledWith(msg);
    expect(setup1.send).toHaveBeenCalledWith(msg);
    expect(setup2.send).toHaveBeenCalledWith(msg);
  });

  test('setup event only notifies the socket itself', () => {
    const socket = { send: jest.fn() };
    const setup1 = { role: 'setup', send: jest.fn() };
    const wss = { clients: new Set([setup1]) };

    registerSocket(socket, 'def', 'setup', wss);

    const msg = JSON.stringify({ event: 'setup_success', id: 'def' });
    expect(socket.role).toBe('setup');
    expect(socket.send).toHaveBeenCalledWith(msg);
    expect(setup1.send).not.toHaveBeenCalled();
  });
});
