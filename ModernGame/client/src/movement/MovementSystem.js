/**
 * Quake-style movement system with air control and strafe jumping
 * Based on Quake 3 movement physics
 */

class MovementSystem {
    constructor() {
        // Movement constants (tuned for feel)
        this.maxSpeed = 320;           // Base max speed
        this.airAccel = 10;            // Air acceleration
        this.groundAccel = 10;         // Ground acceleration
        this.friction = 6;             // Ground friction
        this.airFriction = 0.1;        // Air friction
        this.jumpSpeed = 270;          // Jump velocity
        this.gravity = 800;            // Gravity force
        this.strafeAccel = 1.1;        // Strafe acceleration multiplier
        
        // Player state
        this.velocity = { x: 0, y: 0, z: 0 };
        this.position = { x: 0, y: 0, z: 0 };
        this.onGround = false;
        this.wishDir = { x: 0, y: 0, z: 0 };
        this.wishSpeed = 0;
        
        // Input state
        this.input = {
            forward: 0,
            right: 0,
            jump: false,
            crouch: false
        };
        
        // Performance tracking
        this.stats = {
            maxSpeed: 0,
            airTime: 0,
            strafeJumps: 0
        };
    }
    
    /**
     * Update movement for one frame
     * @param {number} deltaTime - Time since last frame in seconds
     * @param {Object} input - Input state {forward, right, jump, crouch}
     * @param {Object} camera - Camera object for yaw
     */
    update(deltaTime, input, camera) {
        this.input = input;
        
        // Calculate wish direction based on input and camera yaw
        this.calculateWishDirection(camera.yaw);
        
        // Apply gravity
        this.velocity.y -= this.gravity * deltaTime;
        
        // Ground movement
        if (this.onGround) {
            this.groundMove(deltaTime);
        } else {
            this.airMove(deltaTime);
        }
        
        // Apply velocity to position
        this.position.x += this.velocity.x * deltaTime;
        this.position.y += this.velocity.y * deltaTime;
        this.position.z += this.velocity.z * deltaTime;
        
        // Update ground state (simplified - would need collision detection)
        this.onGround = this.position.y <= 0;
        if (this.onGround) {
            this.position.y = 0;
            this.velocity.y = 0;
        }
        
        // Update stats
        this.updateStats(deltaTime);
    }
    
    /**
     * Calculate wish direction from input and camera yaw
     */
    calculateWishDirection(yaw) {
        const forward = this.input.forward;
        const right = this.input.right;
        
        // Convert yaw to radians
        const yawRad = yaw * Math.PI / 180;
        
        // Calculate forward and right vectors
        const forwardX = Math.sin(yawRad);
        const forwardZ = Math.cos(yawRad);
        const rightX = Math.cos(yawRad);
        const rightZ = -Math.sin(yawRad);
        
        // Combine input with vectors
        this.wishDir.x = forward * forwardX + right * rightX;
        this.wishDir.z = forward * forwardZ + right * rightZ;
        this.wishDir.y = 0;
        
        // Normalize wish direction
        const length = Math.sqrt(this.wishDir.x * this.wishDir.x + this.wishDir.z * this.wishDir.z);
        if (length > 0) {
            this.wishDir.x /= length;
            this.wishDir.z /= length;
        }
        
        this.wishSpeed = length * this.maxSpeed;
    }
    
    /**
     * Ground movement with friction and acceleration
     */
    groundMove(deltaTime) {
        // Apply friction
        const speed = Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.z * this.velocity.z);
        if (speed > 0) {
            const friction = Math.max(0, speed - this.friction * deltaTime);
            const scale = friction / speed;
            this.velocity.x *= scale;
            this.velocity.z *= scale;
        }
        
        // Apply acceleration
        this.accelerate(deltaTime, this.groundAccel);
        
        // Jump
        if (this.input.jump) {
            this.velocity.y = this.jumpSpeed;
            this.onGround = false;
        }
    }
    
    /**
     * Air movement with air control
     */
    airMove(deltaTime) {
        // Air control - allows changing direction in air
        const currentSpeed = Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.z * this.velocity.z);
        if (currentSpeed > 0 && this.wishSpeed > 0) {
            // Calculate how much we can change direction
            const airControl = Math.min(this.airAccel * deltaTime, 1.0);
            
            // Strafe jumping - gain speed by strafing
            if (this.isStrafeJumping()) {
                this.accelerate(deltaTime, this.airAccel * this.strafeAccel);
                this.stats.strafeJumps++;
            } else {
                this.accelerate(deltaTime, this.airAccel);
            }
        }
        
        // Apply air friction
        this.velocity.x *= (1 - this.airFriction * deltaTime);
        this.velocity.z *= (1 - this.airFriction * deltaTime);
    }
    
    /**
     * Apply acceleration in wish direction
     */
    accelerate(deltaTime, accel) {
        if (this.wishSpeed <= 0) return;
        
        const currentSpeed = this.velocity.x * this.wishDir.x + this.velocity.z * this.wishDir.z;
        const addSpeed = this.wishSpeed - currentSpeed;
        
        if (addSpeed <= 0) return;
        
        const accelSpeed = accel * deltaTime * this.wishSpeed;
        if (accelSpeed > addSpeed) {
            accelSpeed = addSpeed;
        }
        
        this.velocity.x += accelSpeed * this.wishDir.x;
        this.velocity.z += accelSpeed * this.wishDir.z;
    }
    
    /**
     * Check if player is performing a strafe jump
     */
    isStrafeJumping() {
        if (!this.input.forward && !this.input.right) return false;
        
        const speed = Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.z * this.velocity.z);
        return speed > this.maxSpeed * 0.8; // High speed threshold
    }
    
    /**
     * Update performance statistics
     */
    updateStats(deltaTime) {
        const speed = Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.z * this.velocity.z);
        this.stats.maxSpeed = Math.max(this.stats.maxSpeed, speed);
        
        if (!this.onGround) {
            this.stats.airTime += deltaTime;
        } else {
            this.stats.airTime = 0;
        }
    }
    
    /**
     * Get current movement state
     */
    getState() {
        return {
            position: { ...this.position },
            velocity: { ...this.velocity },
            onGround: this.onGround,
            speed: Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.z * this.velocity.z),
            stats: { ...this.stats }
        };
    }
    
    /**
     * Set movement state (for networking)
     */
    setState(state) {
        this.position = { ...state.position };
        this.velocity = { ...state.velocity };
        this.onGround = state.onGround;
    }
    
    /**
     * Reset movement system
     */
    reset() {
        this.velocity = { x: 0, y: 0, z: 0 };
        this.position = { x: 0, y: 0, z: 0 };
        this.onGround = false;
        this.stats = { maxSpeed: 0, airTime: 0, strafeJumps: 0 };
    }
}

export default MovementSystem;