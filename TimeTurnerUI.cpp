#include "TimeTurnerUI.h"
#include <algorithm>
#include <sstream>

// TimeTurnerUI implementation
TimeTurnerUI::TimeTurnerUI(TimeTurner* turner) 
    : timeTurner(turner), isVisible(false), isExpanded(false),
      selectedEra(Era::MODERN), selectedHazard(Hazard::NONE),
      cityLivingEnabled(true), hazardIntensity(1.0f) {
    createUIElements();
}

TimeTurnerUI::~TimeTurnerUI() {
    // Cleanup
}

void TimeTurnerUI::createUIElements() {
    elements.clear();
    
    // Era selection
    elements.push_back({"era_modern", "Modern Era", true, true, "Modern"});
    elements.push_back({"era_wasteland", "Wasteland Era", true, true, "Wasteland"});
    elements.push_back({"era_frontier", "Frontier Era", true, true, "Frontier"});
    elements.push_back({"era_westworld", "Westworld Era", true, true, "Westworld"});
    
    // Hazard selection
    elements.push_back({"hazard_none", "No Hazard", true, true, "None"});
    elements.push_back({"hazard_nuclear", "Nuclear Fallout", true, true, "Nuclear"});
    elements.push_back({"hazard_warzone", "Warzone", true, true, "Warzone"});
    elements.push_back({"hazard_storm", "Storm", true, true, "Storm"});
    elements.push_back({"hazard_plague", "Plague", true, true, "Plague"});
    
    // City toggle
    elements.push_back({"city_toggle", "City Living", true, true, "Enabled"});
    
    // Hazard intensity
    elements.push_back({"intensity_slider", "Hazard Intensity", true, true, "1.0"});
    
    // Presets
    elements.push_back({"preset_fallback", "Fallout Wasteland", true, true, "fallout_wasteland"});
    elements.push_back({"preset_cod", "CoD Warzone", true, true, "cod_warzone"});
    elements.push_back({"preset_rdr", "RDR Frontier", true, true, "rdr_frontier"});
    elements.push_back({"preset_westworld", "Westworld Simulation", true, true, "westworld_simulation"});
    elements.push_back({"preset_gta", "GTA Chaos", true, true, "gta_chaos"});
    elements.push_back({"preset_cowboy_br", "Cowboy BR Nuclear", true, true, "cowboy_br_nuclear"});
    elements.push_back({"preset_casino", "Westworld Casino", true, true, "westworld_casino"});
}

void TimeTurnerUI::render() {
    if (!isVisible) return;
    
    // Render main UI panel
    std::cout << "=== TIME-TURNER CONTROL PANEL ===" << std::endl;
    
    if (isExpanded) {
        // Full expanded UI
        std::cout << "Era Selection:" << std::endl;
        std::cout << "  [ ] Modern    [ ] Wasteland    [ ] Frontier    [ ] Westworld" << std::endl;
        
        std::cout << "Hazard Overlay:" << std::endl;
        std::cout << "  [ ] None    [ ] Nuclear    [ ] Warzone    [ ] Storm    [ ] Plague" << std::endl;
        
        std::cout << "Environment:" << std::endl;
        std::cout << "  [ ] City Living" << std::endl;
        
        std::cout << "Hazard Intensity: " << hazardIntensity << std::endl;
        
        std::cout << "Presets:" << std::endl;
        std::cout << "  [Fallout Wasteland] [CoD Warzone] [RDR Frontier] [Westworld]" << std::endl;
        std::cout << "  [GTA Chaos] [Cowboy BR Nuclear] [Westworld Casino]" << std::endl;
        
        std::cout << "Actions:" << std::endl;
        std::cout << "  [Randomize] [Reset] [Apply]" << std::endl;
    } else {
        // Compact UI
        std::cout << "Era: " << getEraName(selectedEra) << " | ";
        std::cout << "Hazard: " << getHazardName(selectedHazard) << " | ";
        std::cout << "City: " << (cityLivingEnabled ? "Yes" : "No") << std::endl;
    }
}

void TimeTurnerUI::update(float deltaTime) {
    if (!isVisible) return;
    
    // Update UI state based on current TimeTurner settings
    WorldSettings currentSettings = timeTurner->current();
    
    if (currentSettings.era != selectedEra) {
        selectedEra = currentSettings.era;
        updateUIState();
    }
    
    if (currentSettings.hazard != selectedHazard) {
        selectedHazard = currentSettings.hazard;
        updateUIState();
    }
    
    if (currentSettings.cityLiving != cityLivingEnabled) {
        cityLivingEnabled = currentSettings.cityLiving;
        updateUIState();
    }
    
    if (currentSettings.hazardIntensity != hazardIntensity) {
        hazardIntensity = currentSettings.hazardIntensity;
        updateUIState();
    }
}

void TimeTurnerUI::setVisible(bool visible) {
    isVisible = visible;
}

void TimeTurnerUI::setExpanded(bool expanded) {
    isExpanded = expanded;
}

void TimeTurnerUI::setOnEraChanged(EraChangeCallback callback) {
    onEraChanged = callback;
}

void TimeTurnerUI::setOnHazardChanged(HazardChangeCallback callback) {
    onHazardChanged = callback;
}

void TimeTurnerUI::setOnCityToggled(CityToggleCallback callback) {
    onCityToggled = callback;
}

void TimeTurnerUI::setOnPresetSelected(PresetSelectCallback callback) {
    onPresetSelected = callback;
}

void TimeTurnerUI::loadPreset(const std::string& presetName) {
    timeTurner->applyPreset(presetName);
    selectedPreset = presetName;
    updateUIState();
}

void TimeTurnerUI::savePreset(const std::string& presetName) {
    // Save current settings as a preset
    // This would integrate with your save system
}

std::vector<std::string> TimeTurnerUI::getAvailablePresets() const {
    return timeTurner->getAvailablePresets();
}

void TimeTurnerUI::randomizeSettings() {
    timeTurner->randomizeSettings();
    updateUIState();
}

void TimeTurnerUI::resetToDefaults() {
    timeTurner->resetToDefaults();
    updateUIState();
}

void TimeTurnerUI::applyCurrentSettings() {
    // Apply current UI settings to TimeTurner
    timeTurner->toggleEra(selectedEra);
    timeTurner->toggleHazard(selectedHazard);
    timeTurner->toggleCity(cityLivingEnabled);
    timeTurner->setHazardIntensity(hazardIntensity);
}

void TimeTurnerUI::updateUIState() {
    // Update UI elements based on current state
    for (auto& element : elements) {
        if (element.id.find("era_") == 0) {
            element.enabled = (getEraFromId(element.id) == selectedEra);
        }
        else if (element.id.find("hazard_") == 0) {
            element.enabled = (getHazardFromId(element.id) == selectedHazard);
        }
        else if (element.id == "city_toggle") {
            element.enabled = cityLivingEnabled;
        }
        else if (element.id == "intensity_slider") {
            element.value = std::to_string(hazardIntensity);
        }
    }
}

void TimeTurnerUI::handleEraSelection(const std::string& eraName) {
    Era newEra = getEraFromName(eraName);
    if (newEra != selectedEra) {
        selectedEra = newEra;
        if (onEraChanged) {
            onEraChanged(newEra);
        }
    }
}

void TimeTurnerUI::handleHazardSelection(const std::string& hazardName) {
    Hazard newHazard = getHazardFromName(hazardName);
    if (newHazard != selectedHazard) {
        selectedHazard = newHazard;
        if (onHazardChanged) {
            onHazardChanged(newHazard);
        }
    }
}

void TimeTurnerUI::handleCityToggle(bool enabled) {
    if (enabled != cityLivingEnabled) {
        cityLivingEnabled = enabled;
        if (onCityToggled) {
            onCityToggled(enabled);
        }
    }
}

void TimeTurnerUI::handlePresetSelection(const std::string& presetName) {
    if (onPresetSelected) {
        onPresetSelected(presetName);
    }
}

void TimeTurnerUI::handleHazardIntensityChange(float intensity) {
    if (intensity != hazardIntensity) {
        hazardIntensity = intensity;
        // Update intensity in TimeTurner
        timeTurner->setHazardIntensity(intensity);
    }
}

// Helper functions
std::string TimeTurnerUI::getEraName(Era era) const {
    switch (era) {
        case Era::MODERN: return "Modern";
        case Era::WASTELAND: return "Wasteland";
        case Era::FRONTIER: return "Frontier";
        case Era::WESTWORLD: return "Westworld";
        default: return "Unknown";
    }
}

std::string TimeTurnerUI::getHazardName(Hazard hazard) const {
    switch (hazard) {
        case Hazard::NONE: return "None";
        case Hazard::NUCLEAR_FALLOUT: return "Nuclear Fallout";
        case Hazard::WARZONE: return "Warzone";
        case Hazard::STORM: return "Storm";
        case Hazard::PLAGUE: return "Plague";
        default: return "Unknown";
    }
}

Era TimeTurnerUI::getEraFromName(const std::string& name) const {
    if (name == "Modern") return Era::MODERN;
    if (name == "Wasteland") return Era::WASTELAND;
    if (name == "Frontier") return Era::FRONTIER;
    if (name == "Westworld") return Era::WESTWORLD;
    return Era::MODERN;
}

Hazard TimeTurnerUI::getHazardFromName(const std::string& name) const {
    if (name == "None") return Hazard::NONE;
    if (name == "Nuclear") return Hazard::NUCLEAR_FALLOUT;
    if (name == "Warzone") return Hazard::WARZONE;
    if (name == "Storm") return Hazard::STORM;
    if (name == "Plague") return Hazard::PLAGUE;
    return Hazard::NONE;
}

Era TimeTurnerUI::getEraFromId(const std::string& id) const {
    if (id == "era_modern") return Era::MODERN;
    if (id == "era_wasteland") return Era::WASTELAND;
    if (id == "era_frontier") return Era::FRONTIER;
    if (id == "era_westworld") return Era::WESTWORLD;
    return Era::MODERN;
}

Hazard TimeTurnerUI::getHazardFromId(const std::string& id) const {
    if (id == "hazard_none") return Hazard::NONE;
    if (id == "hazard_nuclear") return Hazard::NUCLEAR_FALLOUT;
    if (id == "hazard_warzone") return Hazard::WARZONE;
    if (id == "hazard_storm") return Hazard::STORM;
    if (id == "hazard_plague") return Hazard::PLAGUE;
    return Hazard::NONE;
}

// TimeTurnerConsole implementation
TimeTurnerConsole::TimeTurnerConsole(TimeTurner* turner) 
    : timeTurner(turner), isVisible(false) {
}

void TimeTurnerConsole::render() {
    if (!isVisible) return;
    
    std::cout << "=== TIME-TURNER CONSOLE ===" << std::endl;
    std::cout << "Current Settings: " << timeTurner->current().getDescription() << std::endl;
    std::cout << "Commands: help, era <name>, hazard <name>, city <on/off>, intensity <0-1>, randomize, reset, status" << std::endl;
    std::cout << "> ";
}

void TimeTurnerConsole::handleInput(const std::string& input) {
    commandHistory.push_back(input);
    executeCommand(input);
}

void TimeTurnerConsole::setVisible(bool visible) {
    isVisible = visible;
}

void TimeTurnerConsole::executeCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "help") {
        help();
    }
    else if (cmd == "era") {
        std::string eraName;
        iss >> eraName;
        setEra(eraName);
    }
    else if (cmd == "hazard") {
        std::string hazardName;
        iss >> hazardName;
        setHazard(hazardName);
    }
    else if (cmd == "city") {
        std::string cityState;
        iss >> cityState;
        toggleCity();
    }
    else if (cmd == "intensity") {
        float intensity;
        iss >> intensity;
        setIntensity(intensity);
    }
    else if (cmd == "randomize") {
        randomize();
    }
    else if (cmd == "reset") {
        reset();
    }
    else if (cmd == "status") {
        status();
    }
    else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}

void TimeTurnerConsole::help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - Show this help" << std::endl;
    std::cout << "  era <name> - Set era (modern, wasteland, frontier, westworld)" << std::endl;
    std::cout << "  hazard <name> - Set hazard (none, nuclear, warzone, storm, plague)" << std::endl;
    std::cout << "  city <on/off> - Toggle city living" << std::endl;
    std::cout << "  intensity <0-1> - Set hazard intensity" << std::endl;
    std::cout << "  randomize - Randomize all settings" << std::endl;
    std::cout << "  reset - Reset to defaults" << std::endl;
    std::cout << "  status - Show current status" << std::endl;
}

void TimeTurnerConsole::setEra(const std::string& eraName) {
    Era era = Era::MODERN;
    if (eraName == "modern") era = Era::MODERN;
    else if (eraName == "wasteland") era = Era::WASTELAND;
    else if (eraName == "frontier") era = Era::FRONTIER;
    else if (eraName == "westworld") era = Era::WESTWORLD;
    
    timeTurner->toggleEra(era);
    std::cout << "Era set to: " << eraName << std::endl;
}

void TimeTurnerConsole::setHazard(const std::string& hazardName) {
    Hazard hazard = Hazard::NONE;
    if (hazardName == "none") hazard = Hazard::NONE;
    else if (hazardName == "nuclear") hazard = Hazard::NUCLEAR_FALLOUT;
    else if (hazardName == "warzone") hazard = Hazard::WARZONE;
    else if (hazardName == "storm") hazard = Hazard::STORM;
    else if (hazardName == "plague") hazard = Hazard::PLAGUE;
    
    timeTurner->toggleHazard(hazard);
    std::cout << "Hazard set to: " << hazardName << std::endl;
}

void TimeTurnerConsole::toggleCity() {
    WorldSettings current = timeTurner->current();
    timeTurner->toggleCity(!current.cityLiving);
    std::cout << "City living: " << (!current.cityLiving ? "ON" : "OFF") << std::endl;
}

void TimeTurnerConsole::setIntensity(float intensity) {
    timeTurner->setHazardIntensity(intensity);
    std::cout << "Hazard intensity set to: " << intensity << std::endl;
}

void TimeTurnerConsole::randomize() {
    timeTurner->randomizeSettings();
    std::cout << "Settings randomized" << std::endl;
}

void TimeTurnerConsole::reset() {
    timeTurner->resetToDefaults();
    std::cout << "Settings reset to defaults" << std::endl;
}

void TimeTurnerConsole::status() {
    WorldSettings current = timeTurner->current();
    std::cout << "Current Status:" << std::endl;
    std::cout << "  Era: " << current.getDescription() << std::endl;
    std::cout << "  City Living: " << (current.cityLiving ? "ON" : "OFF") << std::endl;
    std::cout << "  Hazard Intensity: " << current.hazardIntensity << std::endl;
    std::cout << "  Time Shift: " << (current.enableTimeShift ? "ON" : "OFF") << std::endl;
}