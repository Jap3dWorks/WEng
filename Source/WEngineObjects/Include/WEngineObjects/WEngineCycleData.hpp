#pragma once

class WLevel;

/**
 * Each cycle of the while loop from the engine related data.
 */
struct WEngineCycleData {
    float DeltaTime{999};
    float TotalTime{0};
    WLevel * level{nullptr};
};
