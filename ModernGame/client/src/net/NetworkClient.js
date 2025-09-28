/**
 * Network client with binary protocol, client prediction, and reconciliation
 * Implements the protocol specification for real-time multiplayer
 */

class NetworkClient {
    constructor() {
        this.socket = null;
        this.connected = false;
        this.playerId = null;
        
        // Input buffering for prediction
        this.inputBuffer = [];
        this.maxInputBuffer = 9; // 150ms at 60 FPS
        this.lastProcessedInput = 0;
        
        // Snapshot interpolation
        this.snapshots = [];
        this.maxSnapshots = 2;
        this.interpolationBuffer = 0.1; // 100ms buffer
        
        // Network state
        this.serverTime = 0;
        this.clientTime = 0;
        this.ping = 0;
        this.lastPingTime = 0;
        
        // Entity management
        this.entities = new Map();
        this.localEntity = null;
        
        // Network events
        this.eventHandlers = new Map();
        
        // Protocol constants
        this.PROTOCOL_VERSION = 0x01;
        this.INPUT_SIZE = 6;
        this.SNAPSHOT_HEADER_SIZE = 3;
        this.ENTITY_SIZE = 11;
    }
    
    /**
     * Connect to server
     * @param {string} url - Server URL
     * @param {number} port - Server port
     */
    async connect(url, port) {
        try {
            this.socket = new WebSocket(`ws://${url}:${port}`);
            
            this.socket.onopen = () => {
                console.log('Connected to server');
                this.connected = true;
                this.emit('connected');
            };
            
            this.socket.onmessage = (event) => {
                this.handleMessage(event.data);
            };
            
            this.socket.onclose = () => {
                console.log('Disconnected from server');
                this.connected = false;
                this.emit('disconnected');
            };
            
            this.socket.onerror = (error) => {
                console.error('Network error:', error);
                this.emit('error', error);
            };
            
        } catch (error) {
            console.error('Failed to connect:', error);
            throw error;
        }
    }
    
    /**
     * Disconnect from server
     */
    disconnect() {
        if (this.socket) {
            this.socket.close();
            this.socket = null;
        }
        this.connected = false;
    }
    
    /**
     * Send input to server
     * @param {Object} input - Input state
     */
    sendInput(input) {
        if (!this.connected) return;
        
        // Create input packet
        const packet = this.createInputPacket(input);
        
        // Store in buffer for prediction
        this.inputBuffer.push({
            input: { ...input },
            timestamp: Date.now(),
            packet: packet
        });
        
        // Keep buffer size manageable
        if (this.inputBuffer.length > this.maxInputBuffer) {
            this.inputBuffer.shift();
        }
        
        // Send to server
        this.socket.send(packet);
    }
    
    /**
     * Create binary input packet
     * @param {Object} input - Input state
     * @returns {ArrayBuffer} Binary packet
     */
    createInputPacket(input) {
        const buffer = new ArrayBuffer(this.INPUT_SIZE);
        const view = new DataView(buffer);
        
        let offset = 0;
        
        // Version
        view.setUint8(offset, this.PROTOCOL_VERSION);
        offset += 1;
        
        // Yaw (int16)
        view.setInt16(offset, Math.round(input.yaw * 32767 / 180), true);
        offset += 2;
        
        // Movement X (int8)
        view.setInt8(offset, Math.round(input.ax * 127));
        offset += 1;
        
        // Movement Z (int8)
        view.setInt8(offset, Math.round(input.az * 127));
        offset += 1;
        
        // Jump (uint8)
        view.setUint8(offset, input.jump ? 1 : 0);
        offset += 1;
        
        // Fire (uint8)
        view.setUint8(offset, input.fire ? 1 : 0);
        offset += 1;
        
        return buffer;
    }
    
    /**
     * Handle incoming message from server
     * @param {ArrayBuffer} data - Binary data
     */
    handleMessage(data) {
        const view = new DataView(data);
        const version = view.getUint8(0);
        
        if (version !== this.PROTOCOL_VERSION) {
            console.warn('Protocol version mismatch');
            return;
        }
        
        // Check if it's a snapshot
        if (data.byteLength > this.INPUT_SIZE) {
            this.handleSnapshot(data);
        } else {
            this.handlePong(data);
        }
    }
    
    /**
     * Handle snapshot from server
     * @param {ArrayBuffer} data - Snapshot data
     */
    handleSnapshot(data) {
        const view = new DataView(data);
        let offset = 1; // Skip version
        
        // Get entity count
        const count = view.getUint16(offset, true);
        offset += 2;
        
        // Parse entities
        const entities = [];
        for (let i = 0; i < count; i++) {
            const entity = this.parseEntity(view, offset);
            entities.push(entity);
            offset += this.ENTITY_SIZE;
        }
        
        // Store snapshot
        const snapshot = {
            timestamp: Date.now(),
            entities: entities,
            serverTime: this.serverTime
        };
        
        this.snapshots.push(snapshot);
        
        // Keep only recent snapshots
        if (this.snapshots.length > this.maxSnapshots) {
            this.snapshots.shift();
        }
        
        // Update entities
        this.updateEntities(entities);
        
        // Reconcile with local state
        this.reconcile();
    }
    
    /**
     * Parse entity from snapshot
     * @param {DataView} view - Data view
     * @param {number} offset - Current offset
     * @returns {Object} Entity data
     */
    parseEntity(view, offset) {
        return {
            idHash: view.getUint16(offset, true),
            x: view.getInt16(offset + 2, true) / 100, // Scale to world units
            z: view.getInt16(offset + 4, true) / 100,
            y: view.getInt16(offset + 6, true) / 100,
            yaw: view.getInt16(offset + 8, true) * 180 / 32767, // Scale to degrees
            flags: view.getUint8(offset + 10)
        };
    }
    
    /**
     * Update local entities from snapshot
     * @param {Array} entities - Entity array
     */
    updateEntities(entities) {
        for (const entity of entities) {
            this.entities.set(entity.idHash, entity);
            
            // Update local entity if it's us
            if (entity.idHash === this.playerId) {
                this.localEntity = entity;
            }
        }
    }
    
    /**
     * Reconcile local state with server
     */
    reconcile() {
        if (!this.localEntity || this.inputBuffer.length === 0) {
            return;
        }
        
        // Find the input that corresponds to this snapshot
        const snapshotTime = this.snapshots[this.snapshots.length - 1].timestamp;
        const inputIndex = this.findInputForTime(snapshotTime);
        
        if (inputIndex >= 0) {
            // Reapply inputs from this point forward
            this.reapplyInputs(inputIndex);
        }
    }
    
    /**
     * Find input for given timestamp
     * @param {number} timestamp - Target timestamp
     * @returns {number} Input index
     */
    findInputForTime(timestamp) {
        for (let i = 0; i < this.inputBuffer.length; i++) {
            if (this.inputBuffer[i].timestamp >= timestamp) {
                return i;
            }
        }
        return -1;
    }
    
    /**
     * Reapply inputs from given index
     * @param {number} startIndex - Starting input index
     */
    reapplyInputs(startIndex) {
        // This would integrate with your movement system
        // to reapply inputs and correct any discrepancies
        console.log(`Reapplying inputs from index ${startIndex}`);
    }
    
    /**
     * Handle pong from server
     * @param {ArrayBuffer} data - Pong data
     */
    handlePong(data) {
        const view = new DataView(data);
        const serverTime = view.getUint32(1, true);
        
        this.ping = Date.now() - this.lastPingTime;
        this.serverTime = serverTime;
        this.clientTime = Date.now();
        
        this.emit('ping', this.ping);
    }
    
    /**
     * Send ping to server
     */
    sendPing() {
        if (!this.connected) return;
        
        const buffer = new ArrayBuffer(5);
        const view = new DataView(buffer);
        
        view.setUint8(0, this.PROTOCOL_VERSION);
        view.setUint32(1, Date.now(), true);
        
        this.lastPingTime = Date.now();
        this.socket.send(buffer);
    }
    
    /**
     * Get interpolated entity position
     * @param {number} entityId - Entity ID
     * @returns {Object} Interpolated position
     */
    getInterpolatedPosition(entityId) {
        if (this.snapshots.length < 2) {
            return this.entities.get(entityId);
        }
        
        const now = Date.now();
        const targetTime = now - this.interpolationBuffer * 1000;
        
        // Find snapshots to interpolate between
        let fromSnapshot = null;
        let toSnapshot = null;
        
        for (let i = 0; i < this.snapshots.length - 1; i++) {
            if (this.snapshots[i].timestamp <= targetTime && 
                this.snapshots[i + 1].timestamp >= targetTime) {
                fromSnapshot = this.snapshots[i];
                toSnapshot = this.snapshots[i + 1];
                break;
            }
        }
        
        if (!fromSnapshot || !toSnapshot) {
            return this.entities.get(entityId);
        }
        
        // Interpolate between snapshots
        const alpha = (targetTime - fromSnapshot.timestamp) / 
                     (toSnapshot.timestamp - fromSnapshot.timestamp);
        
        const fromEntity = fromSnapshot.entities.find(e => e.idHash === entityId);
        const toEntity = toSnapshot.entities.find(e => e.idHash === entityId);
        
        if (!fromEntity || !toEntity) {
            return this.entities.get(entityId);
        }
        
        return {
            x: fromEntity.x + (toEntity.x - fromEntity.x) * alpha,
            y: fromEntity.y + (toEntity.y - fromEntity.y) * alpha,
            z: fromEntity.z + (toEntity.z - fromEntity.z) * alpha,
            yaw: fromEntity.yaw + (toEntity.yaw - fromEntity.yaw) * alpha,
            flags: toEntity.flags
        };
    }
    
    /**
     * Add event handler
     * @param {string} event - Event name
     * @param {Function} handler - Event handler
     */
    on(event, handler) {
        if (!this.eventHandlers.has(event)) {
            this.eventHandlers.set(event, []);
        }
        this.eventHandlers.get(event).push(handler);
    }
    
    /**
     * Emit event
     * @param {string} event - Event name
     * @param {...any} args - Event arguments
     */
    emit(event, ...args) {
        if (this.eventHandlers.has(event)) {
            for (const handler of this.eventHandlers.get(event)) {
                handler(...args);
            }
        }
    }
    
    /**
     * Get current network state
     */
    getState() {
        return {
            connected: this.connected,
            ping: this.ping,
            entities: Array.from(this.entities.values()),
            snapshots: this.snapshots.length,
            inputBuffer: this.inputBuffer.length
        };
    }
}

export default NetworkClient;