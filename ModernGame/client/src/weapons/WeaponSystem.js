/**
 * Weapon system with hitscan, recoil patterns, and damage zones
 * Supports multiple weapon types with different characteristics
 */

class WeaponSystem {
    constructor() {
        this.weapons = this.initializeWeapons();
        this.currentWeapon = 'rifle';
        this.fireCooldown = 0;
        this.recoilPattern = [];
        this.spreadBloom = 0;
        this.maxSpread = 0;
        this.lastFireTime = 0;
        
        // Damage zones
        this.damageZones = {
            head: { multiplier: 4.0, armor: 0 },
            body: { multiplier: 1.0, armor: 1 },
            limb: { multiplier: 0.75, armor: 0.5 }
        };
        
        // Recoil patterns (up, side) in degrees
        this.recoilPatterns = {
            rifle: [
                { up: 0, side: 0 },
                { up: -1, side: 0 },
                { up: -2, side: 1 },
                { up: -3, side: -1 },
                { up: -4, side: 2 },
                { up: -5, side: -2 },
                { up: -6, side: 1 },
                { up: -7, side: -1 },
                { up: -8, side: 0 },
                { up: -9, side: 1 }
            ]
        };
    }
    
    /**
     * Initialize weapon definitions
     */
    initializeWeapons() {
        return {
            rifle: {
                name: 'Assault Rifle',
                damage: 30,
                rpm: 600,
                spread: 0.5,
                maxSpread: 3.0,
                spreadIncrease: 0.3,
                spreadDecay: 2.0,
                range: 1000,
                penetration: 1,
                recoil: 0.8
            },
            pistol: {
                name: 'Pistol',
                damage: 25,
                rpm: 300,
                spread: 0.2,
                maxSpread: 2.0,
                spreadIncrease: 0.5,
                spreadDecay: 3.0,
                range: 500,
                penetration: 0,
                recoil: 0.3
            }
        };
    }
    
    /**
     * Update weapon system
     * @param {number} deltaTime - Time since last frame
     */
    update(deltaTime) {
        // Update fire cooldown
        if (this.fireCooldown > 0) {
            this.fireCooldown -= deltaTime;
        }
        
        // Decay spread bloom
        if (this.spreadBloom > 0) {
            const weapon = this.weapons[this.currentWeapon];
            this.spreadBloom = Math.max(0, this.spreadBloom - weapon.spreadDecay * deltaTime);
        }
    }
    
    /**
     * Fire weapon
     * @param {Object} origin - Fire origin {x, y, z}
     * @param {Object} direction - Fire direction {x, y, z}
     * @param {boolean} ads - Aim down sights
     * @returns {Object} Fire result with bullet data
     */
    fire(origin, direction, ads = false) {
        const weapon = this.weapons[this.currentWeapon];
        
        // Check fire cooldown
        if (this.fireCooldown > 0) {
            return null;
        }
        
        // Calculate spread
        const baseSpread = ads ? weapon.spread * 0.5 : weapon.spread;
        const totalSpread = baseSpread + this.spreadBloom;
        const finalSpread = Math.min(totalSpread, weapon.maxSpread);
        
        // Apply spread to direction
        const spreadDirection = this.applySpread(direction, finalSpread);
        
        // Calculate recoil
        const recoil = this.calculateRecoil();
        
        // Update state
        this.fireCooldown = 60.0 / weapon.rpm; // Convert RPM to cooldown
        this.spreadBloom = Math.min(weapon.maxSpread, this.spreadBloom + weapon.spreadIncrease);
        this.lastFireTime = Date.now();
        
        // Generate bullet data
        const bullet = {
            id: this.generateBulletId(),
            origin: { ...origin },
            direction: spreadDirection,
            damage: weapon.damage,
            range: weapon.range,
            penetration: weapon.penetration,
            timestamp: Date.now(),
            seed: Math.random()
        };
        
        return {
            bullet,
            recoil,
            spread: finalSpread,
            weapon: this.currentWeapon
        };
    }
    
    /**
     * Apply spread to fire direction
     */
    applySpread(direction, spread) {
        // Convert spread from degrees to radians
        const spreadRad = spread * Math.PI / 180;
        
        // Generate random angles
        const angle = Math.random() * 2 * Math.PI;
        const distance = Math.random() * spreadRad;
        
        // Create spread vector
        const spreadX = Math.sin(angle) * distance;
        const spreadY = Math.cos(angle) * distance;
        
        // Apply spread to direction (simplified - would need proper 3D math)
        return {
            x: direction.x + spreadX,
            y: direction.y + spreadY,
            z: direction.z
        };
    }
    
    /**
     * Calculate recoil for current shot
     */
    calculateRecoil() {
        const weapon = this.weapons[this.currentWeapon];
        const pattern = this.recoilPatterns[this.currentWeapon];
        
        if (!pattern || pattern.length === 0) {
            return { up: 0, side: 0 };
        }
        
        // Get current shot in pattern
        const shotIndex = Math.min(this.recoilPattern.length, pattern.length - 1);
        const recoil = pattern[shotIndex];
        
        // Add to pattern
        this.recoilPattern.push(recoil);
        
        // Apply weapon recoil multiplier
        return {
            up: recoil.up * weapon.recoil,
            side: recoil.side * weapon.recoil
        };
    }
    
    /**
     * Process hit and calculate damage
     * @param {Object} hit - Hit data {target, distance, hitZone}
     * @returns {Object} Damage result
     */
    processHit(hit) {
        const weapon = this.weapons[this.currentWeapon];
        const zone = this.damageZones[hit.hitZone];
        
        // Calculate base damage
        let damage = weapon.damage;
        
        // Apply distance falloff
        const falloff = Math.max(0, 1 - (hit.distance / weapon.range));
        damage *= falloff;
        
        // Apply zone multiplier
        damage *= zone.multiplier;
        
        // Apply armor
        const armorDamage = Math.max(0, damage - zone.armor);
        
        return {
            damage: Math.round(damage),
            armorDamage: Math.round(armorDamage),
            hitZone: hit.hitZone,
            distance: hit.distance,
            critical: zone.multiplier > 2.0
        };
    }
    
    /**
     * Switch weapon
     */
    switchWeapon(weaponName) {
        if (this.weapons[weaponName]) {
            this.currentWeapon = weaponName;
            this.recoilPattern = [];
            this.spreadBloom = 0;
        }
    }
    
    /**
     * Get current weapon info
     */
    getCurrentWeapon() {
        return {
            ...this.weapons[this.currentWeapon],
            name: this.currentWeapon,
            fireCooldown: this.fireCooldown,
            spreadBloom: this.spreadBloom
        };
    }
    
    /**
     * Generate unique bullet ID
     */
    generateBulletId() {
        return Date.now() + Math.random() * 1000;
    }
    
    /**
     * Reset weapon system
     */
    reset() {
        this.fireCooldown = 0;
        this.recoilPattern = [];
        this.spreadBloom = 0;
        this.lastFireTime = 0;
    }
}

export default WeaponSystem;