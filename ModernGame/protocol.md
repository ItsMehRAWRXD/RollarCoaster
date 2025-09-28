# Game Protocol Specification

## Version
Protocol version: 0x01

## Client → Server (Input)
**Size**: 6 bytes
**Frequency**: 60 Hz (16.67ms intervals)

```
struct ClientInput {
    uint8_t version;     // 0x01
    int16_t yaw;         // -32768 to 32767 (scaled to -180 to 180 degrees)
    int8_t ax;           // -128 to 127 (left/right movement)
    int8_t az;           // -128 to 127 (forward/back movement)
    uint8_t jump;        // 0 or 1
    uint8_t fire;        // 0 or 1
};
```

## Server → Client (Snapshot)
**Size**: Variable (2 + count * 11 bytes)
**Frequency**: 20 Hz (50ms intervals)

```
struct Snapshot {
    uint8_t version;     // 0x01
    uint16_t count;      // Number of entities
    Entity entities[count];
};

struct Entity {
    uint16_t idHash;     // Entity ID hash
    int16_t x;           // Position X (scaled to world units)
    int16_t z;           // Position Z (scaled to world units)
    int16_t y;           // Position Y (scaled to world units)
    int16_t yaw;         // Rotation (scaled to -180 to 180 degrees)
    uint8_t flags;       // Bit flags: alive(0), shooting(1), jumping(2), etc.
};
```

## Hit Registration
**Size**: 8 bytes

```
struct HitEvent {
    uint8_t version;     // 0x01
    uint32_t shotId;     // Unique shot identifier
    uint16_t targetId;   // Target entity ID
    uint8_t damage;      // Damage amount
    uint8_t hitZone;     // 0=head, 1=body, 2=limb
};
```

## Interest Grid
- **Radius**: 60-80 meters
- **Grid Size**: 10x10 meter cells
- **Update Rate**: 5 Hz for distant entities, 20 Hz for nearby
- **Bandwidth Target**: < 30 kB/s per client

## Client Prediction
- **Input Buffer**: 150ms (9 frames at 60 FPS)
- **Reconciliation**: Reapply inputs when snapshot arrives
- **Interpolation**: 2 snapshot buffer (100ms total)
- **Rewind**: Server stores 150ms of entity poses for hit validation

## Example Packet Flow

### Client Input Example
```
Version: 0x01
Yaw: 45 degrees = 4096 (45 * 32767 / 360)
Ax: Right = 127
Az: Forward = 127
Jump: 0
Fire: 1
```

### Server Snapshot Example
```
Version: 0x01
Count: 3
Entity 1: ID=12345, Pos=(100,50,200), Yaw=45, Flags=0x01 (alive)
Entity 2: ID=12346, Pos=(150,50,250), Yaw=90, Flags=0x03 (alive+shooting)
Entity 3: ID=12347, Pos=(200,50,300), Yaw=180, Flags=0x05 (alive+jumping)
```

## Bandwidth Optimization
- **Compression**: Delta encoding for position changes
- **Culling**: Only send entities within interest radius
- **Batching**: Group multiple updates per packet
- **Prediction**: Client-side movement prediction to reduce server load