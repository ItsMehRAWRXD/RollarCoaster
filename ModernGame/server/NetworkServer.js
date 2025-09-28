/**
 * Network server with binary protocol and interest grid
 * Handles client connections, input processing, and snapshot broadcasting
 */

const WebSocket = require('ws');
const EventEmitter = require('events');

class NetworkServer extends EventEmitter {
    constructor(port = 8080) {
        super();
        
        this.port = port;
        this.wss = null;
        this.clients = new Map();
        this.entities = new Map();
        this.nextEntityId = 1;
        
        // Interest grid
        this.gridSize = 10; // 10 meter cells
        this.interestRadius = 80; // 80 meter radius
        this.grid = new Map();
        
        // Snapshot broadcasting
        this.snapshotRate = 20; // 20 Hz
        this.lastSnapshot = 0;
        this.snapshotInterval = 1000 / this.snapshotRate;
        
        // Hit validation
        this.poseHistory = new Map(); // Store 150ms of poses
        this.historyDuration = 150; // 150ms
        
        // Protocol constants
        this.PROTOCOL_VERSION = 0x01;
        this.INPUT_SIZE = 6;
        this.SNAPSHOT_HEADER_SIZE = 3;
        this.ENTITY_SIZE = 11;
    }
    
    /**
     * Start the server
     */
    start() {
        this.wss = new WebSocket.Server({ port: this.port });
        
        this.wss.on('connection', (ws, req) => {
            this.handleConnection(ws, req);
        });
        
        // Start snapshot broadcasting
        this.startSnapshotLoop();
        
        // Start pose history cleanup
        this.startHistoryCleanup();
        
        console.log(`Network server started on port ${this.port}`);
    }
    
    /**
     * Stop the server
     */
    stop() {
        if (this.wss) {
            this.wss.close();
            this.wss = null;
        }
    }
    
    /**
     * Handle new client connection
     * @param {WebSocket} ws - WebSocket connection
     * @param {Object} req - Request object
     */
    handleConnection(ws, req) {
        const clientId = this.generateClientId();
        const client = {
            id: clientId,
            ws: ws,
            connected: true,
            lastPing: Date.now(),
            entityId: null,
            position: { x: 0, y: 0, z: 0 },
            yaw: 0,
            flags: 0
        };
        
        this.clients.set(clientId, client);
        
        // Create entity for client
        const entityId = this.createEntity(client);
        client.entityId = entityId;
        
        console.log(`Client ${clientId} connected`);
        this.emit('clientConnected', client);
        
        // Handle messages
        ws.on('message', (data) => {
            this.handleMessage(clientId, data);
        });
        
        // Handle disconnection
        ws.on('close', () => {
            this.handleDisconnection(clientId);
        });
        
        // Handle errors
        ws.on('error', (error) => {
            console.error(`Client ${clientId} error:`, error);
        });
    }
    
    /**
     * Handle client disconnection
     * @param {string} clientId - Client ID
     */
    handleDisconnection(clientId) {
        const client = this.clients.get(clientId);
        if (client) {
            client.connected = false;
            
            // Remove entity
            if (client.entityId) {
                this.entities.delete(client.entityId);
            }
            
            // Remove from interest grid
            this.removeFromGrid(clientId);
            
            console.log(`Client ${clientId} disconnected`);
            this.emit('clientDisconnected', client);
        }
        
        this.clients.delete(clientId);
    }
    
    /**
     * Handle incoming message from client
     * @param {string} clientId - Client ID
     * @param {Buffer} data - Message data
     */
    handleMessage(clientId, data) {
        const client = this.clients.get(clientId);
        if (!client) return;
        
        const view = new DataView(data);
        const version = view.getUint8(0);
        
        if (version !== this.PROTOCOL_VERSION) {
            console.warn(`Protocol version mismatch from client ${clientId}`);
            return;
        }
        
        // Check if it's input or ping
        if (data.byteLength === this.INPUT_SIZE) {
            this.handleInput(clientId, data);
        } else if (data.byteLength === 5) {
            this.handlePing(clientId, data);
        }
    }
    
    /**
     * Handle client input
     * @param {string} clientId - Client ID
     * @param {Buffer} data - Input data
     */
    handleInput(clientId, data) {
        const client = this.clients.get(clientId);
        if (!client) return;
        
        const view = new DataView(data);
        let offset = 1; // Skip version
        
        // Parse input
        const input = {
            yaw: view.getInt16(offset, true) * 180 / 32767,
            ax: view.getInt8(offset + 2) / 127,
            az: view.getInt8(offset + 3) / 127,
            jump: view.getUint8(offset + 4) === 1,
            fire: view.getUint8(offset + 5) === 1
        };
        
        // Update client state
        this.updateClientState(client, input);
        
        // Store in pose history
        this.storePoseHistory(client, input);
        
        // Emit input event
        this.emit('clientInput', client, input);
    }
    
    /**
     * Handle client ping
     * @param {string} clientId - Client ID
     * @param {Buffer} data - Ping data
     */
    handlePing(clientId, data) {
        const client = this.clients.get(clientId);
        if (!client) return;
        
        // Send pong back
        const buffer = new ArrayBuffer(5);
        const view = new DataView(buffer);
        
        view.setUint8(0, this.PROTOCOL_VERSION);
        view.setUint32(1, Date.now(), true);
        
        client.ws.send(buffer);
        client.lastPing = Date.now();
    }
    
    /**
     * Update client state from input
     * @param {Object} client - Client object
     * @param {Object} input - Input state
     */
    updateClientState(client, input) {
        // Update position based on input (simplified)
        const speed = 5; // 5 units per second
        const deltaTime = 1/60; // Assume 60 FPS
        
        // Calculate movement direction
        const forward = Math.sin(input.yaw * Math.PI / 180);
        const right = Math.cos(input.yaw * Math.PI / 180);
        
        // Apply movement
        client.position.x += (input.ax * right + input.az * forward) * speed * deltaTime;
        client.position.z += (input.ax * forward - input.az * right) * speed * deltaTime;
        client.position.y += input.jump ? 10 : 0; // Simple jump
        
        // Update yaw
        client.yaw = input.yaw;
        
        // Update flags
        client.flags = 0;
        if (input.jump) client.flags |= 0x01;
        if (input.fire) client.flags |= 0x02;
        
        // Update entity
        if (client.entityId) {
            const entity = this.entities.get(client.entityId);
            if (entity) {
                entity.x = client.position.x;
                entity.y = client.position.y;
                entity.z = client.position.z;
                entity.yaw = client.yaw;
                entity.flags = client.flags;
            }
        }
    }
    
    /**
     * Store pose history for hit validation
     * @param {Object} client - Client object
     * @param {Object} input - Input state
     */
    storePoseHistory(client, input) {
        const now = Date.now();
        const history = this.poseHistory.get(client.id) || [];
        
        // Add current pose
        history.push({
            timestamp: now,
            position: { ...client.position },
            yaw: client.yaw,
            input: { ...input }
        });
        
        // Remove old poses
        const cutoff = now - this.historyDuration;
        while (history.length > 0 && history[0].timestamp < cutoff) {
            history.shift();
        }
        
        this.poseHistory.set(client.id, history);
    }
    
    /**
     * Start snapshot broadcasting loop
     */
    startSnapshotLoop() {
        setInterval(() => {
            this.broadcastSnapshots();
        }, this.snapshotInterval);
    }
    
    /**
     * Broadcast snapshots to all clients
     */
    broadcastSnapshots() {
        for (const [clientId, client] of this.clients) {
            if (!client.connected) continue;
            
            // Get entities in interest radius
            const nearbyEntities = this.getNearbyEntities(client);
            
            // Create snapshot
            const snapshot = this.createSnapshot(nearbyEntities);
            
            // Send to client
            if (client.ws.readyState === WebSocket.OPEN) {
                client.ws.send(snapshot);
            }
        }
    }
    
    /**
     * Get entities near client
     * @param {Object} client - Client object
     * @returns {Array} Nearby entities
     */
    getNearbyEntities(client) {
        const nearby = [];
        
        for (const [entityId, entity] of this.entities) {
            const distance = this.getDistance(client.position, entity);
            if (distance <= this.interestRadius) {
                nearby.push({
                    idHash: entityId,
                    x: Math.round(entity.x * 100),
                    z: Math.round(entity.z * 100),
                    y: Math.round(entity.y * 100),
                    yaw: Math.round(entity.yaw * 32767 / 180),
                    flags: entity.flags
                });
            }
        }
        
        return nearby;
    }
    
    /**
     * Create snapshot packet
     * @param {Array} entities - Entity array
     * @returns {Buffer} Snapshot packet
     */
    createSnapshot(entities) {
        const size = this.SNAPSHOT_HEADER_SIZE + entities.length * this.ENTITY_SIZE;
        const buffer = new ArrayBuffer(size);
        const view = new DataView(buffer);
        
        let offset = 0;
        
        // Version
        view.setUint8(offset, this.PROTOCOL_VERSION);
        offset += 1;
        
        // Entity count
        view.setUint16(offset, entities.length, true);
        offset += 2;
        
        // Entity data
        for (const entity of entities) {
            view.setUint16(offset, entity.idHash, true);
            offset += 2;
            view.setInt16(offset, entity.x, true);
            offset += 2;
            view.setInt16(offset, entity.z, true);
            offset += 2;
            view.setInt16(offset, entity.y, true);
            offset += 2;
            view.setInt16(offset, entity.yaw, true);
            offset += 2;
            view.setUint8(offset, entity.flags);
            offset += 1;
        }
        
        return Buffer.from(buffer);
    }
    
    /**
     * Create entity for client
     * @param {Object} client - Client object
     * @returns {number} Entity ID
     */
    createEntity(client) {
        const entityId = this.nextEntityId++;
        const entity = {
            id: entityId,
            x: client.position.x,
            y: client.position.y,
            z: client.position.z,
            yaw: client.yaw,
            flags: 0
        };
        
        this.entities.set(entityId, entity);
        return entityId;
    }
    
    /**
     * Remove client from interest grid
     * @param {string} clientId - Client ID
     */
    removeFromGrid(clientId) {
        // Implementation would depend on your grid system
        console.log(`Removed client ${clientId} from grid`);
    }
    
    /**
     * Get distance between two positions
     * @param {Object} pos1 - First position
     * @param {Object} pos2 - Second position
     * @returns {number} Distance
     */
    getDistance(pos1, pos2) {
        const dx = pos1.x - pos2.x;
        const dy = pos1.y - pos2.y;
        const dz = pos1.z - pos2.z;
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * Generate unique client ID
     * @returns {string} Client ID
     */
    generateClientId() {
        return Math.random().toString(36).substr(2, 9);
    }
    
    /**
     * Start pose history cleanup
     */
    startHistoryCleanup() {
        setInterval(() => {
            const now = Date.now();
            const cutoff = now - this.historyDuration;
            
            for (const [clientId, history] of this.poseHistory) {
                while (history.length > 0 && history[0].timestamp < cutoff) {
                    history.shift();
                }
            }
        }, 1000); // Clean up every second
    }
    
    /**
     * Get server statistics
     */
    getStats() {
        return {
            clients: this.clients.size,
            entities: this.entities.size,
            uptime: Date.now() - this.startTime
        };
    }
}

module.exports = NetworkServer;