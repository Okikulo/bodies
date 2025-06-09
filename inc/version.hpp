#pragma once

// Auto-generated file - do not edit manually
// Generated from version.toml

#define PROJECT_VERSION "0.0.1"
#define PROJECT_VERSION_MAJOR 0
#define PROJECT_VERSION_MINOR 0
#define PROJECT_VERSION_PATCH 1

// Helper functions
#include <string>

namespace Version {
    inline std::string get_version() {
        return PROJECT_VERSION;
    }
    
    inline int get_major() {
        return PROJECT_VERSION_MAJOR;
    }
    
    inline int get_minor() {
        return PROJECT_VERSION_MINOR;
    }
    
    inline int get_patch() {
        return PROJECT_VERSION_PATCH;
    }
    
    inline std::string get_full_info() {
        return "Version " + get_version() + " (Built on " + __DATE__ + ")";
    }
}
