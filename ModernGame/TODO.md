# Modern Game Development Roadmap

## Phase 0 — Lock the Core (Today)
**Goal**: One map, one mode, buttery movement, solid 60 FPS on potato.

### Movement & Feel
- [ ] Tune MOVE.maxSpeed/airAccel/dodge until strafes feel predictable
- [ ] **Acceptance**: Figure-8 around two boxes for 30s at 60 FPS; no hitch on jumps or dodges

### Gun Loop (Single Rifle + Hitscan)
- [ ] One recoil curve + spread bloom, head/body/limb damage
- [ ] **Acceptance**: Bot at 15m dies in 6–8 body shots; headshots consistent; hip vs ADS spread distinct

### Bomb Mode Only
- [ ] Plant/defuse with clear HUD states; round win banner; MR12 economy minimal
- [ ] **Acceptance**: 12 fast rounds play without UI desync; money never goes negative; defuse time consistent

### Performance
- [ ] Cap draw calls; ensure mipmaps/aniso set; grid/helper off in prod
- [ ] **Acceptance**: 720p @ 60 FPS on integrated graphics, renderScale ≤ 0.8

## Phase 1 — Netcode "Real Mode" (1–2 days)
**Goal**: Online round with 8–16 people on a DO droplet.

### Protocol (Freeze It)
- [ ] Input → server (binary 6B): yaw:int16, ax:int8, az:int8, jump:u8, fire:u8
- [ ] Snapshot ← server (binary): count:uint16; [idHash:uint16, x:int16, z:int16, y:int16, yaw:int16, flags:u8]*count
- [ ] Version byte at start of both directions (e.g., 0x01) so you can evolve

### Client Prediction + Reconciliation
- [ ] Keep last 150ms of your inputs; reapply when snapshot arrives
- [ ] Simple hit rewind at server (store 150ms poses)

### Interest Grid Radius
- [ ] Start with 60–80m; send only nearby entities
- [ ] **Acceptance**: 16 clients see stable interpolation with <150ms buffer; bandwidth per client < 30 kB/s

## Phase 2 — BR or QUADS (Pick One) (2–3 days)
**Goal**: One marquee mode fully playable with humans (bots optional).

### QUADS (Recommended First)
- [ ] Spawn four corners; ticket pool per team; respawn waves every 10s; last team with tickets wins
- [ ] **Acceptance**: 4×10 players, deaths drain tickets, win/lose deterministic, scoreboard correct

### OR BR (Non-Drop)
- [ ] Ground spawns on edges; circle phases; damage outside; simple loot table
- [ ] **Acceptance**: 40 players survive to final circle; "alive" counter never desyncs; ring timer exact

## Phase 3 — UX Polish (1–2 days)
- [ ] Minimal HUD: ammo, health/armor, objective banner, compass ticks (N/E/S/W), alive counter
- [ ] Options: FOV, render scale, mouse sens; Save to JSON (later Steam Cloud)
- [ ] Mobile: enable touch sticks, scale UI, pointer-lock fallback

## Phase 4 — Steam-Ready (1–2 days)
- [ ] Electron build → smoke test on Win/Linux
- [ ] Steam store demo build: Bomb + chosen marquee mode, offline bots + online toggle
- [ ] Achievements stub (local): First Plant, First Win, 10 Frags. Hook later to Steamworks

## Hard Acceptance Bar (What "Done" Means)
- [ ] 60 FPS stable on a mid laptop, 30+ on mobile with renderScale 0.6
- [ ] <= 150 ms interpolation buffer; no rubber-banding in 16-player test
- [ ] Round loop never desyncs: everyone sees same winner, same money
- [ ] Crash-free 20 minutes in each mode

## Micro-Milestones (Commit Messages You Can Actually Ship)
- [ ] `feat(core): lock Quake movement & hitscan; remove grid helper in prod`
- [ ] `feat(net): add uWS server, binary snapshots, 150ms interp buffer`
- [ ] `feat(mode): QUADS with tickets & waves; scoreboard + banners`
- [ ] `perf(client): instanced props, cached materials, lower overdraw`
- [ ] `build(steam): electron-builder win/linux; add appid stub`