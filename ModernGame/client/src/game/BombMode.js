/**
 * Bomb plant/defuse mode with MR12 economy
 * Implements Counter-Strike style bomb mechanics
 */

class BombMode {
    constructor() {
        this.gameState = 'waiting'; // waiting, playing, ended
        this.roundState = 'freeze'; // freeze, active, ended
        this.roundTime = 115; // 1:55 round time
        this.plantTime = 3.0; // 3 seconds to plant
        this.defuseTime = 5.0; // 5 seconds to defuse (no kit)
        this.defuseTimeWithKit = 2.5; // 2.5 seconds with kit
        
        // Teams
        this.teams = {
            terrorists: {
                name: 'Terrorists',
                players: [],
                score: 0,
                money: 800
            },
            counterTerrorists: {
                name: 'Counter-Terrorists',
                players: [],
                score: 0,
                money: 800
            }
        };
        
        // Economy (MR12)
        this.economy = {
            round: 1,
            maxRounds: 12,
            halfTime: 6,
            winMoney: 3250,
            lossMoney: 1400,
            bombPlantMoney: 300,
            defuseMoney: 300
        };
        
        // Bomb state
        this.bomb = {
            planted: false,
            site: null, // 'A' or 'B'
            planter: null,
            planterId: null,
            plantTime: 0,
            defuseTime: 0,
            defuser: null,
            defuserId: null,
            defuseKit: false,
            explosionTime: 40 // 40 seconds after plant
        };
        
        // Sites
        this.sites = {
            A: { x: 100, y: 0, z: 100, radius: 50 },
            B: { x: -100, y: 0, z: -100, radius: 50 }
        };
        
        // Round results
        this.roundResults = [];
    }
    
    /**
     * Start new round
     */
    startRound() {
        this.roundState = 'freeze';
        this.roundTime = 115;
        this.bomb = {
            planted: false,
            site: null,
            planter: null,
            planterId: null,
            plantTime: 0,
            defuseTime: 0,
            defuser: null,
            defuserId: null,
            defuseKit: false,
            explosionTime: 40
        };
        
        // Give money based on economy
        this.distributeMoney();
        
        console.log(`Round ${this.economy.round} starting - Freeze time`);
        
        // Start freeze time (3 seconds)
        setTimeout(() => {
            this.roundState = 'active';
            console.log('Round active - Fight!');
        }, 3000);
    }
    
    /**
     * Distribute money based on economy rules
     */
    distributeMoney() {
        const isFirstRound = this.economy.round === 1;
        const isHalfTime = this.economy.round === this.economy.halfTime + 1;
        
        // Base money
        let tMoney = isFirstRound ? 800 : 1400;
        let ctMoney = isFirstRound ? 800 : 1400;
        
        // Add win/loss bonuses from previous round
        if (this.roundResults.length > 0) {
            const lastResult = this.roundResults[this.roundResults.length - 1];
            if (lastResult.winner === 'terrorists') {
                tMoney += this.economy.winMoney;
                ctMoney += this.economy.lossMoney;
            } else if (lastResult.winner === 'counterTerrorists') {
                ctMoney += this.economy.winMoney;
                tMoney += this.economy.lossMoney;
            }
        }
        
        // Half-time money boost
        if (isHalfTime) {
            tMoney += 1000;
            ctMoney += 1000;
        }
        
        this.teams.terrorists.money = Math.min(tMoney, 16000);
        this.teams.counterTerrorists.money = Math.min(ctMoney, 16000);
    }
    
    /**
     * Update round timer
     * @param {number} deltaTime - Time since last frame
     */
    update(deltaTime) {
        if (this.roundState === 'active') {
            this.roundTime -= deltaTime;
            
            // Check for round end conditions
            if (this.roundTime <= 0) {
                this.endRound('time', 'counterTerrorists');
            }
            
            // Update bomb if planted
            if (this.bomb.planted) {
                this.updateBomb(deltaTime);
            }
        }
    }
    
    /**
     * Update bomb state
     */
    updateBomb(deltaTime) {
        if (!this.bomb.planted) return;
        
        // Check if defusing
        if (this.bomb.defuser) {
            this.bomb.defuseTime += deltaTime;
            const requiredTime = this.bomb.defuseKit ? this.defuseTimeWithKit : this.defuseTime;
            
            if (this.bomb.defuseTime >= requiredTime) {
                this.defuseBomb();
            }
        }
        
        // Check explosion
        const timeSincePlant = Date.now() - this.bomb.plantTime;
        if (timeSincePlant >= this.bomb.explosionTime * 1000) {
            this.explodeBomb();
        }
    }
    
    /**
     * Attempt to plant bomb
     * @param {string} playerId - Player attempting to plant
     * @param {Object} position - Player position
     * @returns {boolean} Success
     */
    plantBomb(playerId, position) {
        if (this.bomb.planted || this.roundState !== 'active') {
            return false;
        }
        
        // Check if player is terrorist
        const player = this.getPlayer(playerId);
        if (!player || player.team !== 'terrorists') {
            return false;
        }
        
        // Check if near bomb site
        const site = this.getNearestSite(position);
        if (!site) {
            return false;
        }
        
        // Start planting
        this.bomb.site = site;
        this.bomb.planter = player;
        this.bomb.planterId = playerId;
        this.bomb.plantTime = Date.now();
        
        console.log(`Player ${playerId} planting bomb at site ${site}`);
        return true;
    }
    
    /**
     * Complete bomb plant
     */
    completePlant() {
        this.bomb.planted = true;
        this.teams.terrorists.money += this.economy.bombPlantMoney;
        console.log(`Bomb planted at site ${this.bomb.site} by ${this.bomb.planterId}`);
    }
    
    /**
     * Attempt to defuse bomb
     * @param {string} playerId - Player attempting to defuse
     * @param {Object} position - Player position
     * @returns {boolean} Success
     */
    defuseBomb(playerId, position) {
        if (!this.bomb.planted || this.roundState !== 'active') {
            return false;
        }
        
        // Check if player is CT
        const player = this.getPlayer(playerId);
        if (!player || player.team !== 'counterTerrorists') {
            return false;
        }
        
        // Check if near bomb
        const distance = this.getDistance(position, this.getBombPosition());
        if (distance > 50) { // 50 unit radius
            return false;
        }
        
        // Start defusing
        this.bomb.defuser = player;
        this.bomb.defuserId = playerId;
        this.bomb.defuseTime = 0;
        this.bomb.defuseKit = player.hasDefuseKit || false;
        
        console.log(`Player ${playerId} defusing bomb (kit: ${this.bomb.defuseKit})`);
        return true;
    }
    
    /**
     * Complete bomb defuse
     */
    defuseBomb() {
        this.bomb.planted = false;
        this.teams.counterTerrorists.money += this.economy.defuseMoney;
        console.log(`Bomb defused by ${this.bomb.defuserId}`);
        this.endRound('defuse', 'counterTerrorists');
    }
    
    /**
     * Explode bomb
     */
    explodeBomb() {
        console.log(`Bomb exploded at site ${this.bomb.site}`);
        this.endRound('explosion', 'terrorists');
    }
    
    /**
     * End round with result
     * @param {string} reason - Round end reason
     * @param {string} winner - Winning team
     */
    endRound(reason, winner) {
        this.roundState = 'ended';
        
        const result = {
            round: this.economy.round,
            reason,
            winner,
            time: this.roundTime,
            bombPlanted: this.bomb.planted
        };
        
        this.roundResults.push(result);
        
        // Update scores
        if (winner === 'terrorists') {
            this.teams.terrorists.score++;
        } else {
            this.teams.counterTerrorists.score++;
        }
        
        console.log(`Round ${this.economy.round} ended: ${reason} - ${winner} wins`);
        
        // Check for match end
        if (this.teams.terrorists.score >= 13 || this.teams.counterTerrorists.score >= 13) {
            this.endMatch();
        } else {
            // Start next round after 5 seconds
            this.economy.round++;
            setTimeout(() => {
                this.startRound();
            }, 5000);
        }
    }
    
    /**
     * End match
     */
    endMatch() {
        this.gameState = 'ended';
        const winner = this.teams.terrorists.score > this.teams.counterTerrorists.score ? 'terrorists' : 'counterTerrorists';
        console.log(`Match ended - ${winner} wins!`);
    }
    
    /**
     * Get nearest bomb site to position
     */
    getNearestSite(position) {
        let nearest = null;
        let minDistance = Infinity;
        
        for (const [name, site] of Object.entries(this.sites)) {
            const distance = this.getDistance(position, site);
            if (distance < site.radius && distance < minDistance) {
                minDistance = distance;
                nearest = name;
            }
        }
        
        return nearest;
    }
    
    /**
     * Get bomb position
     */
    getBombPosition() {
        if (!this.bomb.planted || !this.bomb.site) {
            return null;
        }
        return this.sites[this.bomb.site];
    }
    
    /**
     * Get distance between two positions
     */
    getDistance(pos1, pos2) {
        const dx = pos1.x - pos2.x;
        const dy = pos1.y - pos2.y;
        const dz = pos1.z - pos2.z;
        return Math.sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * Get player by ID
     */
    getPlayer(playerId) {
        // This would integrate with your player management system
        return null; // Placeholder
    }
    
    /**
     * Get current game state
     */
    getState() {
        return {
            gameState: this.gameState,
            roundState: this.roundState,
            roundTime: Math.max(0, this.roundTime),
            round: this.economy.round,
            maxRounds: this.economy.maxRounds,
            teams: this.teams,
            bomb: this.bomb,
            results: this.roundResults
        };
    }
    
    /**
     * Reset bomb mode
     */
    reset() {
        this.gameState = 'waiting';
        this.roundState = 'freeze';
        this.economy.round = 1;
        this.teams.terrorists.score = 0;
        this.teams.counterTerrorists.score = 0;
        this.roundResults = [];
        this.bomb = {
            planted: false,
            site: null,
            planter: null,
            planterId: null,
            plantTime: 0,
            defuseTime: 0,
            defuser: null,
            defuserId: null,
            defuseKit: false,
            explosionTime: 40
        };
    }
}

export default BombMode;