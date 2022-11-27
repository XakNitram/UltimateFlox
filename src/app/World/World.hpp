#pragma once

#include "pch.hpp"

static constexpr float worldBound = 450.0f;

inline Vector calculateBounds(float aspect) {
    return {
        aspect >= 1.0f ? worldBound * aspect : worldBound,
        aspect < 1.0f ? worldBound * aspect : worldBound
    };
}
