import { WebSocket } from 'ws';

export type Role = string;

export interface ManagedWebSocket extends WebSocket {
  id?: string;
  role?: Role;
}

export class ClientManager {
  private clients: Map<Role, Map<string, ManagedWebSocket>> = new Map();

  register(id: string, role: Role, socket: ManagedWebSocket) {
    if (!this.clients.has(role)) {
      this.clients.set(role, new Map());
    }
    this.clients.get(role)!.set(id, socket);
    socket.id = id;
    socket.role = role;
  }

  unregister(socket: ManagedWebSocket) {
    const { id, role } = socket;
    if (!id || !role) return;
    const roleMap = this.clients.get(role);
    roleMap?.delete(id);
    if (roleMap && roleMap.size === 0) {
      this.clients.delete(role);
    }
  }

  broadcast(role: Role, message: string) {
    const roleMap = this.clients.get(role);
    if (!roleMap) return;
    for (const client of roleMap.values()) {
      client.send(message);
    }
  }

  send(role: Role, id: string, message: string) {
    const client = this.clients.get(role)?.get(id);
    client?.send(message);
  }
}
