#!/bin/bash

set -e

if [ ! -f "version.toml" ]; then
    echo "Error: version.toml not found!"
    exit 1
fi

VERSION=$(grep 'version = ' version.toml | sed 's/.*"\(.*\)".*/\1/')

if [ -z "$VERSION" ]; then
    echo "Error: Could not parse version from version.toml"
    exit 1
fi

IFS='.' read -r MAJOR MINOR PATCH <<< "$VERSION"

mkdir -p inc

cat > inc/version.hpp << EOF
#pragma once

// Auto-generated file - do not edit manually
// Generated from version.toml

#define PROJECT_VERSION "${VERSION}"
#define PROJECT_VERSION_MAJOR ${MAJOR}
#define PROJECT_VERSION_MINOR ${MINOR}
#define PROJECT_VERSION_PATCH ${PATCH}

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
EOF

echo "Generated inc/version.hpp with version ${VERSION}"
