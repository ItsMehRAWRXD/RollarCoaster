# Time-Turner Meta-System

A flexible sandbox architecture that allows dynamic switching between different eras, themes, and hazards in a single game engine. This system enables players and servers to flip between different game modes and atmospheres like ticking boxes.

## 🎯 Core Concept

The Time-Turner system transforms your game engine into a time-shifting sandbox director. Instead of being locked into one game type, you can dynamically switch between:

- **Modern Era**: Contemporary settings (GTA, CoD, MoH)
- **Wasteland Era**: Post-apocalyptic (Fallout)
- **Frontier Era**: 1500s-1800s frontier (RDR, Westworld)
- **Westworld Era**: Meta-simulation layer with mixed eras

Each era can be overlaid with hazards like nuclear fallout, warzones, storms, or plagues, creating unique combinations like "Cowboy BR with nuclear fallout" or "Westworld casino deathmatch."

## 🏗️ Architecture

### Core Components

1. **TimeTurner**: Main director class that manages era/hazard switching
2. **EraMapGenerator**: Generates era-appropriate biomes and locations
3. **TimeTurnerUI**: User interface for toggling settings
4. **TimeTurnerConsole**: Debug console for rapid testing

### Key Features

- **Dynamic Era Switching**: Change the entire world theme on the fly
- **Hazard Overlays**: Apply environmental threats to any era
- **City/Rural Toggle**: Switch between urban and wilderness settings
- **Preset System**: Quick setup for common scenarios
- **NPC Adaptation**: NPCs automatically adjust to current era
- **Map Generation**: Procedural generation based on era/hazard combinations

## 🚀 Quick Start

### Basic Usage

```cpp
#include "TimeTurner.h"

// Create time-turner instance
TimeTurner timeTurner;

// Switch to different eras
timeTurner.toggleEra(Era::WASTELAND);
timeTurner.toggleHazard(Hazard::NUCLEAR_FALLOUT);
timeTurner.toggleCity(false);

// Get current settings
WorldSettings current = timeTurner.current();
std::cout << "Current: " << current.getDescription() << std::endl;
```

### Using Presets

```cpp
// Apply preset configurations
timeTurner.applyPreset("fallout_wasteland");
timeTurner.applyPreset("cod_warzone");
timeTurner.applyPreset("rdr_frontier");
timeTurner.applyPreset("westworld_simulation");
```

### Map Generation

```cpp
#include "EraMapGenerator.h"

EraMapGenerator mapGenerator;
MapGenerationParams params;
params.worldSettings = timeTurner.current();
params.mapSize = 1000;

GeneratedMap map = mapGenerator.generateMap(params);
```

## 🎮 Game Modes & Scenarios

### Survival Mode
- **Fallout Wasteland**: `Era::WASTELAND + Hazard::NUCLEAR_FALLOUT`
- **Post-Apocalyptic Western**: `Era::FRONTIER + Hazard::NUCLEAR_FALLOUT`

### PvP Modes
- **Urban Warzone**: `Era::MODERN + Hazard::WARZONE + cityLiving=true`
- **Cowboy BR**: `Era::FRONTIER + cityLiving=false`
- **Nuclear Cowboy BR**: `Era::FRONTIER + Hazard::NUCLEAR_FALLOUT`

### Campaign Modes
- **Time-Spanning Stories**: Rotate eras to tell epic narratives
- **Westworld Meta**: `Era::WESTWORLD` with mixed-era elements

### Mutators
- **GoldenEye Paintball**: Apply to any era
- **Slappers Only**: Melee combat in any setting
- **Radiation Mode**: Add nuclear hazard to any era

## 🎛️ UI System

### Main Control Panel

```
=== TIME-TURNER CONTROL PANEL ===
Era Selection:
  [✓] Modern    [ ] Wasteland    [ ] Frontier    [ ] Westworld

Hazard Overlay:
  [ ] None    [✓] Nuclear    [ ] Warzone    [ ] Storm    [ ] Plague

Environment:
  [✓] City Living

Hazard Intensity: 0.8

Presets:
  [Fallout Wasteland] [CoD Warzone] [RDR Frontier] [Westworld]
  [GTA Chaos] [Cowboy BR Nuclear] [Westworld Casino]

Actions:
  [Randomize] [Reset] [Apply]
```

### Console Commands

```
> era frontier
> hazard nuclear
> city off
> intensity 0.9
> randomize
> status
```

## 🗺️ Map Generation

### Era-Specific Biomes

- **Modern**: Urban skyscrapers, streets, city sprawl
- **Wasteland**: Irradiated ruins, bunkers, wasteland
- **Frontier**: Western towns, saloons, wilderness
- **Westworld**: Mixed-era theme park, host stations

### Hazard Overlays

- **Nuclear Fallout**: Radiation zones, mutant spawns
- **Warzone**: Combat zones, airstrike areas
- **Storm**: Weather hazards, visibility reduction
- **Plague**: Contaminated areas, disease effects

### Special Locations

Each era generates appropriate special locations:

- **Frontier**: Saloons, general stores, sheriff's office
- **Wasteland**: Bunkers, supply caches, mutant lairs
- **Modern**: Police stations, hospitals, military bases
- **Westworld**: Host stations, control rooms, guest areas

## 🤖 NPC System

### Era-Appropriate NPCs

- **Modern**: Civilians, police, military personnel
- **Wasteland**: Raiders, mutants, survivors
- **Frontier**: Cowboys, bandits, settlers
- **Westworld**: Hosts, guests, technicians

### Dynamic Behavior

NPCs automatically adapt to current era and hazards:

- **Hostile NPCs**: Become more aggressive in warzones
- **Radiation Effects**: NPCs may become mutants in irradiated areas
- **Era Transitions**: NPCs change appearance and behavior

## 🔧 Integration

### With Existing Systems

The Time-Turner system is designed to integrate with your existing game systems:

1. **Map Generation**: Extend your current map generator to accept `WorldSettings`
2. **NPC System**: Update NPC spawning to use era-appropriate templates
3. **UI System**: Add time-turner controls to your existing UI
4. **Save System**: Include `WorldSettings` in save files

### Example Integration

```cpp
// In your main game loop
void Game::update(float deltaTime) {
    // Update time-turner
    timeTurner.update(deltaTime);
    
    // Regenerate map if settings changed
    if (timeTurner.needsMapRegeneration()) {
        regenerateMap();
    }
    
    // Update NPCs for current era
    updateNPCsForEra(timeTurner.current().era);
    
    // Apply hazard effects
    applyHazardEffects(timeTurner.current().hazard);
}
```

## 🎯 Use Cases

### Server Administration
- **Dynamic Events**: Switch eras for special events
- **Player Choice**: Let players vote on era/hazard combinations
- **A/B Testing**: Test different configurations

### Single Player
- **Campaign Mode**: Progress through different eras
- **Sandbox Mode**: Experiment with different combinations
- **Challenge Mode**: Survive in extreme era/hazard combinations

### Multiplayer
- **Lobby Configuration**: Set era/hazard before match
- **Mid-Game Changes**: Switch eras during gameplay
- **Tournament Mode**: Predefined era/hazard combinations

## 🚀 Advanced Features

### Custom Themes
```cpp
timeTurner.setCustomTheme("casino");
timeTurner.toggleEra(Era::WESTWORLD);
```

### Randomization
```cpp
timeTurner.randomizeSettings();
```

### Validation
```cpp
if (timeTurner.canTransitionTo(Era::WESTWORLD)) {
    timeTurner.toggleEra(Era::WESTWORLD);
}
```

## 📝 Example Scenarios

### "Fallout Meets Red Dead"
```cpp
timeTurner.toggleEra(Era::FRONTIER);
timeTurner.toggleHazard(Hazard::NUCLEAR_FALLOUT);
timeTurner.toggleCity(false);
// Result: Post-apocalyptic western wasteland
```

### "CoD Urban Warfare"
```cpp
timeTurner.toggleEra(Era::MODERN);
timeTurner.toggleHazard(Hazard::WARZONE);
timeTurner.toggleCity(true);
// Result: Urban warzone with skyscraper fights
```

### "Westworld Casino"
```cpp
timeTurner.toggleEra(Era::WESTWORLD);
timeTurner.setCustomTheme("casino");
// Result: Westworld casino complex
```

## 🔮 Future Extensions

The Time-Turner system is designed to be extensible:

- **New Eras**: Add medieval, sci-fi, or fantasy eras
- **New Hazards**: Add alien invasions, zombie outbreaks, or time rifts
- **Custom Biomes**: Create unique biome types
- **Advanced NPCs**: Add more complex NPC behavior systems
- **Weather Systems**: Integrate with weather simulation
- **Economy Systems**: Era-appropriate trading and resources

## 🎉 Conclusion

The Time-Turner system transforms your game engine into a flexible, dynamic sandbox that can adapt to any scenario. Whether you want to create a post-apocalyptic western, a modern urban warzone, or a Westworld-style simulation, the system provides the tools to make it happen with just a few toggles.

This isn't just one game—it's a time-shifting sandbox director that can create countless unique experiences from a single codebase.