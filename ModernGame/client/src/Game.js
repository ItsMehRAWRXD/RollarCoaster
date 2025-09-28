/**
 * Main game class that orchestrates all systems
 * Implements the core game loop with movement, weapons, and networking
 */

import MovementSystem from './movement/MovementSystem.js';
import WeaponSystem from './weapons/WeaponSystem.js';
import BombMode from './game/BombMode.js';
import NetworkClient from './net/NetworkClient.js';

class Game {
    constructor() {
        // Core systems
        this.movement = new MovementSystem();
        this.weapons = new WeaponSystem();
        this.bombMode = new BombMode();
        this.network = new NetworkClient();
        
        // Game state
        this.gameState = 'menu'; // menu, playing, spectating
        this.localPlayer = null;
        this.players = new Map();
        this.camera = {
            yaw: 0,
            pitch: 0,
            position: { x: 0, y: 0, z: 0 }
        };
        
        // Input state
        this.input = {
            forward: 0,
            right: 0,
            jump: false,
            fire: false,
            ads: false
        };
        
        // Performance tracking
        this.stats = {
            fps: 0,
            frameTime: 0,
            drawCalls: 0,
            triangles: 0
        };
        
        // Game loop
        this.lastFrameTime = 0;
        this.targetFPS = 60;
        this.frameInterval = 1000 / this.targetFPS;
        this.running = false;
        
        // Setup event handlers
        this.setupEventHandlers();
    }
    
    /**
     * Initialize the game
     */
    async initialize() {
        console.log('Initializing game...');
        
        // Initialize systems
        this.movement.reset();
        this.weapons.reset();
        this.bombMode.reset();
        
        // Setup networking
        this.network.on('connected', () => {
            console.log('Connected to server');
            this.gameState = 'playing';
        });
        
        this.network.on('disconnected', () => {
            console.log('Disconnected from server');
            this.gameState = 'menu';
        });
        
        this.network.on('ping', (ping) => {
            console.log(`Ping: ${ping}ms`);
        });
        
        // Start game loop
        this.start();
        
        console.log('Game initialized');
    }
    
    /**
     * Start the game loop
     */
    start() {
        this.running = true;
        this.lastFrameTime = performance.now();
        this.gameLoop();
    }
    
    /**
     * Stop the game loop
     */
    stop() {
        this.running = false;
    }
    
    /**
     * Main game loop
     */
    gameLoop() {
        if (!this.running) return;
        
        const currentTime = performance.now();
        const deltaTime = (currentTime - this.lastFrameTime) / 1000;
        this.lastFrameTime = currentTime;
        
        // Update FPS
        this.stats.fps = 1 / deltaTime;
        this.stats.frameTime = deltaTime * 1000;
        
        // Update systems
        this.update(deltaTime);
        
        // Render
        this.render();
        
        // Continue loop
        requestAnimationFrame(() => this.gameLoop());
    }
    
    /**
     * Update game systems
     * @param {number} deltaTime - Time since last frame
     */
    update(deltaTime) {
        // Update movement
        this.movement.update(deltaTime, this.input, this.camera);
        
        // Update weapons
        this.weapons.update(deltaTime);
        
        // Update bomb mode
        this.bombMode.update(deltaTime);
        
        // Update networking
        this.updateNetworking();
        
        // Update camera
        this.updateCamera();
    }
    
    /**
     * Update networking
     */
    updateNetworking() {
        if (this.network.connected) {
            // Send input to server
            const input = {
                yaw: this.camera.yaw,
                ax: this.input.right,
                az: this.input.forward,
                jump: this.input.jump,
                fire: this.input.fire
            };
            
            this.network.sendInput(input);
            
            // Update local player from network
            const networkState = this.network.getState();
            if (networkState.entities) {
                this.updatePlayers(networkState.entities);
            }
        }
    }
    
    /**
     * Update players from network data
     * @param {Array} entities - Network entities
     */
    updatePlayers(entities) {
        for (const entity of entities) {
            if (!this.players.has(entity.idHash)) {
                this.players.set(entity.idHash, {
                    id: entity.idHash,
                    position: { x: entity.x, y: entity.y, z: entity.z },
                    yaw: entity.yaw,
                    flags: entity.flags
                });
            } else {
                const player = this.players.get(entity.idHash);
                player.position.x = entity.x;
                player.position.y = entity.y;
                player.position.z = entity.z;
                player.yaw = entity.yaw;
                player.flags = entity.flags;
            }
        }
    }
    
    /**
     * Update camera
     */
    updateCamera() {
        // Update camera position from movement
        const movementState = this.movement.getState();
        this.camera.position.x = movementState.position.x;
        this.camera.position.y = movementState.position.y;
        this.camera.position.z = movementState.position.z;
    }
    
    /**
     * Render the game
     */
    render() {
        // Clear screen
        this.clearScreen();
        
        // Render world
        this.renderWorld();
        
        // Render players
        this.renderPlayers();
        
        // Render UI
        this.renderUI();
        
        // Update stats
        this.updateStats();
    }
    
    /**
     * Clear the screen
     */
    clearScreen() {
        // This would integrate with your rendering system
        console.log('Clearing screen');
    }
    
    /**
     * Render the world
     */
    renderWorld() {
        // This would integrate with your world rendering
        console.log('Rendering world');
    }
    
    /**
     * Render players
     */
    renderPlayers() {
        for (const [id, player] of this.players) {
            // Render player at position
            console.log(`Rendering player ${id} at (${player.position.x}, ${player.position.y}, ${player.position.z})`);
        }
    }
    
    /**
     * Render UI
     */
    renderUI() {
        // Render HUD
        this.renderHUD();
        
        // Render bomb mode UI
        this.renderBombUI();
    }
    
    /**
     * Render HUD
     */
    renderHUD() {
        const weapon = this.weapons.getCurrentWeapon();
        const movementState = this.movement.getState();
        const bombState = this.bombMode.getState();
        
        console.log(`HUD: Health=100, Armor=100, Ammo=${weapon.name}, Speed=${movementState.speed.toFixed(1)}`);
    }
    
    /**
     * Render bomb mode UI
     */
    renderBombUI() {
        const bombState = this.bombMode.getState();
        
        if (bombState.roundState === 'active') {
            console.log(`Round ${bombState.round}: ${bombState.roundTime.toFixed(1)}s`);
        }
    }
    
    /**
     * Update performance statistics
     */
    updateStats() {
        // This would integrate with your performance monitoring
        if (this.stats.fps < 50) {
            console.warn(`Low FPS: ${this.stats.fps.toFixed(1)}`);
        }
    }
    
    /**
     * Setup event handlers
     */
    setupEventHandlers() {
        // Keyboard input
        document.addEventListener('keydown', (event) => {
            this.handleKeyDown(event);
        });
        
        document.addEventListener('keyup', (event) => {
            this.handleKeyUp(event);
        });
        
        // Mouse input
        document.addEventListener('mousedown', (event) => {
            this.handleMouseDown(event);
        });
        
        document.addEventListener('mouseup', (event) => {
            this.handleMouseUp(event);
        });
        
        // Mouse movement
        document.addEventListener('mousemove', (event) => {
            this.handleMouseMove(event);
        });
    }
    
    /**
     * Handle key down
     * @param {KeyboardEvent} event - Key event
     */
    handleKeyDown(event) {
        switch (event.code) {
            case 'KeyW':
                this.input.forward = 1;
                break;
            case 'KeyS':
                this.input.forward = -1;
                break;
            case 'KeyA':
                this.input.right = -1;
                break;
            case 'KeyD':
                this.input.right = 1;
                break;
            case 'Space':
                this.input.jump = true;
                break;
            case 'KeyR':
                this.weapons.reload();
                break;
        }
    }
    
    /**
     * Handle key up
     * @param {KeyboardEvent} event - Key event
     */
    handleKeyUp(event) {
        switch (event.code) {
            case 'KeyW':
            case 'KeyS':
                this.input.forward = 0;
                break;
            case 'KeyA':
            case 'KeyD':
                this.input.right = 0;
                break;
            case 'Space':
                this.input.jump = false;
                break;
        }
    }
    
    /**
     * Handle mouse down
     * @param {MouseEvent} event - Mouse event
     */
    handleMouseDown(event) {
        if (event.button === 0) { // Left click
            this.input.fire = true;
            this.handleFire();
        } else if (event.button === 2) { // Right click
            this.input.ads = true;
        }
    }
    
    /**
     * Handle mouse up
     * @param {MouseEvent} event - Mouse event
     */
    handleMouseUp(event) {
        if (event.button === 0) { // Left click
            this.input.fire = false;
        } else if (event.button === 2) { // Right click
            this.input.ads = false;
        }
    }
    
    /**
     * Handle mouse movement
     * @param {MouseEvent} event - Mouse event
     */
    handleMouseMove(event) {
        const sensitivity = 0.1;
        
        this.camera.yaw += event.movementX * sensitivity;
        this.camera.pitch -= event.movementY * sensitivity;
        
        // Clamp pitch
        this.camera.pitch = Math.max(-90, Math.min(90, this.camera.pitch));
    }
    
    /**
     * Handle fire
     */
    handleFire() {
        if (!this.weapons.canFire()) return;
        
        const fireResult = this.weapons.fire(
            this.camera.position,
            this.getFireDirection(),
            this.input.ads
        );
        
        if (fireResult) {
            console.log(`Fired ${fireResult.weapon} with spread ${fireResult.spread.toFixed(2)}`);
            
            // Send hit event to server if we hit something
            // This would integrate with your hit detection system
        }
    }
    
    /**
     * Get fire direction from camera
     * @returns {Object} Fire direction
     */
    getFireDirection() {
        const yawRad = this.camera.yaw * Math.PI / 180;
        const pitchRad = this.camera.pitch * Math.PI / 180;
        
        return {
            x: Math.sin(yawRad) * Math.cos(pitchRad),
            y: Math.sin(pitchRad),
            z: Math.cos(yawRad) * Math.cos(pitchRad)
        };
    }
    
    /**
     * Connect to server
     * @param {string} url - Server URL
     * @param {number} port - Server port
     */
    async connectToServer(url, port) {
        try {
            await this.network.connect(url, port);
            console.log('Connected to server');
        } catch (error) {
            console.error('Failed to connect to server:', error);
        }
    }
    
    /**
     * Get game state
     */
    getState() {
        return {
            gameState: this.gameState,
            players: this.players.size,
            stats: this.stats,
            bombMode: this.bombMode.getState(),
            network: this.network.getState()
        };
    }
}

export default Game;