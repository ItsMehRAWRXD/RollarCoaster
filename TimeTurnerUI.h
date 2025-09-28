#pragma once
#include "TimeTurner.h"
#include <string>
#include <vector>
#include <functional>

// UI callback types
using EraChangeCallback = std::function<void(Era)>;
using HazardChangeCallback = std::function<void(Hazard)>;
using CityToggleCallback = std::function<void(bool)>;
using PresetSelectCallback = std::function<void(const std::string&)>;

// Time-Turner UI System
class TimeTurnerUI {
private:
    TimeTurner* timeTurner;
    bool isVisible;
    bool isExpanded;
    
    // UI State
    Era selectedEra;
    Hazard selectedHazard;
    bool cityLivingEnabled;
    float hazardIntensity;
    std::string selectedPreset;
    
    // Callbacks
    EraChangeCallback onEraChanged;
    HazardChangeCallback onHazardChanged;
    CityToggleCallback onCityToggled;
    PresetSelectCallback onPresetSelected;
    
    // UI Elements
    struct UIElement {
        std::string id;
        std::string label;
        bool enabled;
        bool visible;
        std::string value;
    };
    
    std::vector<UIElement> elements;
    
    // Internal methods
    void createUIElements();
    void updateUIState();
    void handleEraSelection(const std::string& eraName);
    void handleHazardSelection(const std::string& hazardName);
    void handleCityToggle(bool enabled);
    void handlePresetSelection(const std::string& presetName);
    void handleHazardIntensityChange(float intensity);
    
public:
    TimeTurnerUI(TimeTurner* turner);
    ~TimeTurnerUI();
    
    // Main UI methods
    void render();
    void update(float deltaTime);
    void setVisible(bool visible);
    void setExpanded(bool expanded);
    
    // Callback setters
    void setOnEraChanged(EraChangeCallback callback);
    void setOnHazardChanged(HazardChangeCallback callback);
    void setOnCityToggled(CityToggleCallback callback);
    void setOnPresetSelected(PresetSelectCallback callback);
    
    // State getters
    bool getIsVisible() const { return isVisible; }
    bool getIsExpanded() const { return isExpanded; }
    Era getSelectedEra() const { return selectedEra; }
    Hazard getSelectedHazard() const { return selectedHazard; }
    bool getCityLivingEnabled() const { return cityLivingEnabled; }
    float getHazardIntensity() const { return hazardIntensity; }
    
    // Preset management
    void loadPreset(const std::string& presetName);
    void savePreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Quick actions
    void randomizeSettings();
    void resetToDefaults();
    void applyCurrentSettings();
};

// Specialized UI components
class EraSelector {
private:
    std::vector<std::pair<Era, std::string>> eraOptions;
    Era selectedEra;
    EraChangeCallback onEraChanged;
    
public:
    EraSelector();
    void render();
    void setSelectedEra(Era era);
    Era getSelectedEra() const { return selectedEra; }
    void setOnEraChanged(EraChangeCallback callback);
};

class HazardSelector {
private:
    std::vector<std::pair<Hazard, std::string>> hazardOptions;
    Hazard selectedHazard;
    float intensity;
    HazardChangeCallback onHazardChanged;
    std::function<void(float)> onIntensityChanged;
    
public:
    HazardSelector();
    void render();
    void setSelectedHazard(Hazard hazard);
    void setIntensity(float intensity);
    Hazard getSelectedHazard() const { return selectedHazard; }
    float getIntensity() const { return intensity; }
    void setOnHazardChanged(HazardChangeCallback callback);
    void setOnIntensityChanged(std::function<void(float)> callback);
};

class CityToggle {
private:
    bool enabled;
    CityToggleCallback onToggled;
    
public:
    CityToggle();
    void render();
    void setEnabled(bool enabled);
    bool getEnabled() const { return enabled; }
    void setOnToggled(CityToggleCallback callback);
};

class PresetSelector {
private:
    std::vector<std::string> presets;
    std::string selectedPreset;
    PresetSelectCallback onPresetSelected;
    
public:
    PresetSelector();
    void render();
    void setSelectedPreset(const std::string& preset);
    std::string getSelectedPreset() const { return selectedPreset; }
    void setOnPresetSelected(PresetSelectCallback callback);
    void refreshPresets();
};

// Console-style UI for debugging
class TimeTurnerConsole {
private:
    TimeTurner* timeTurner;
    std::vector<std::string> commandHistory;
    std::string currentCommand;
    bool isVisible;
    
public:
    TimeTurnerConsole(TimeTurner* turner);
    void render();
    void handleInput(const std::string& input);
    void setVisible(bool visible);
    
    // Console commands
    void executeCommand(const std::string& command);
    void help();
    void listEras();
    void listHazards();
    void listPresets();
    void setEra(const std::string& eraName);
    void setHazard(const std::string& hazardName);
    void toggleCity();
    void setIntensity(float intensity);
    void randomize();
    void reset();
    void status();
};

// Lobby configuration UI
class LobbyConfigUI {
private:
    TimeTurner* timeTurner;
    bool isVisible;
    bool isHost;
    
    // Lobby settings
    struct LobbySettings {
        std::string gameMode;
        std::string mapSize;
        int maxPlayers;
        bool allowEraChanges;
        bool allowHazardChanges;
        bool allowCityToggle;
    };
    
    LobbySettings lobbySettings;
    
public:
    LobbyConfigUI(TimeTurner* turner);
    void render();
    void setVisible(bool visible);
    void setIsHost(bool host);
    
    // Lobby management
    void createLobby();
    void joinLobby(const std::string& lobbyId);
    void updateLobbySettings();
    void broadcastSettings();
    
    // Settings getters/setters
    LobbySettings getLobbySettings() const { return lobbySettings; }
    void setLobbySettings(const LobbySettings& settings);
};