#pragma once

namespace island::terrain_rock
{
    // Shared GLSL for each vertical projection. Blend evaluated materials, never world
    // coordinates: mix(x,z,abs(x)/(abs(x)+abs(z))) is zero whenever x and z oppose.
    // The footprint includes height, keeping steep faces filtered at grazing distances.
    inline constexpr const char* Shader = R"(
float valueNoise(vec2 p);
struct RockFaceMaterial {
    vec3 color;
    float outcropField;
    float cliffMacroModulation;
};
RockFaceMaterial rockFaceMaterial(vec2 face, float phaseOffset, float facePixelWidth,
                                  float coastalCliff, vec2 macroOffset, vec2 mottleOffset)
{
    float faceAcross = face.x + phaseOffset;
    float projectedMacro = valueNoise(face * vec2(0.055, 0.075) + macroOffset);
    float projectedMottle = valueNoise(face * vec2(0.31, 0.38) + mottleOffset);
    float rockTone = clamp(0.50 + projectedMacro * 0.82
        + projectedMottle * 0.30, 0.0, 1.0);
    vec3 weatheredRock = mix(vec3(0.255, 0.272, 0.268),
                        vec3(0.455, 0.392, 0.300), rockTone);
    vec3 saltWornRock = mix(vec3(0.350, 0.370, 0.350),
                            vec3(0.485, 0.425, 0.330),
                            clamp(0.52 + projectedMacro * 0.76
                                + projectedMottle * 0.34, 0.0, 1.0));
    weatheredRock = mix(weatheredRock, saltWornRock, coastalCliff * 0.62);
    // The close bedding and grain deliberately filter away with footprint size. Keep a
    // much broader, low-contrast geological rhythm for the whole-headland view so the
    // seven-metre face does not collapse into one flat grey curtain at distance.
    float macroStrata = sin(face.y * 1.08 + projectedMacro * 0.68
        + sin(faceAcross * 0.085) * 0.34);
    float drainageField = valueNoise(vec2(faceAcross * 0.092, face.y * 0.052)
        + vec2(-14.6, 7.3));
    float drainageStreak = smoothstep(0.18, 0.70,
        drainageField + projectedMottle * 0.20);
    float warmBed = smoothstep(0.30, 0.82,
        macroStrata + projectedMacro * 0.26);
    weatheredRock = mix(weatheredRock, vec3(0.470, 0.392, 0.292),
        coastalCliff * warmBed * 0.27);
    weatheredRock *= 1.0 + coastalCliff
        * (macroStrata * 0.135 - drainageStreak * 0.155);
    float cliffMacroModulation = coastalCliff
        * (macroStrata * 0.075 - drainageStreak * 0.080);
    weatheredRock *= 1.0 + projectedMottle * 0.25;
    float beddingPhase = face.y * 1.72 + projectedMacro * 0.58
        + sin(faceAcross * 0.11 + projectedMottle * 1.4) * 0.30;
    // Thin soil leaves connected but irregular outcrops, rather than tinting an entire
    // inland hillside as one dark muddy curtain. Low coastal cliffs remain more exposed.
    // Macro colour may span tens of metres, so it must only bias exposure.  Let the
    // three-metre mottle and broken bedding define the actual boundary; otherwise a
    // modest slope turns into one soft, dark oval that resembles a painted stain.
    float outcropField = projectedMacro * 0.18 + projectedMottle * 0.72
        + sin(beddingPhase * 0.47 + faceAcross * 0.035) * 0.16;
    float beddingWave = sin(beddingPhase) + 0.34 * sin(beddingPhase * 2.07 + 0.8);
    float beddingVisibility = 1.0 - smoothstep(0.16, 0.48, facePixelWidth * 2.05);
    float bedding = beddingWave * beddingVisibility;
    float seamDistance = abs(fract(beddingPhase / 6.28318530718) - 0.5);
    float seamWidth = max(0.018, facePixelWidth * 0.035);
    float beddingSeam = 1.0 - smoothstep(0.030, 0.030 + seamWidth, seamDistance);
    float seamBreak = valueNoise(vec2(faceAcross * 0.16, face.y * 0.27)
        + vec2(-6.8, 12.1));
    beddingSeam *= smoothstep(-0.34, 0.16, seamBreak + projectedMottle * 0.45);
    float jointCoordinate = faceAcross * 0.18
        + sin(face.y * 0.21 + projectedMottle * 1.8) * 0.14
        + valueNoise(vec2(face.y * 0.18, faceAcross * 0.065) + vec2(4.9, -8.2)) * 0.70;
    float jointDistance = abs(fract(jointCoordinate) - 0.5);
    float jointWidth = max(0.012, facePixelWidth * 0.022);
    float verticalJoint = 1.0 - smoothstep(0.020, 0.020 + jointWidth, jointDistance);
    float jointBreak = valueNoise(vec2(faceAcross * 0.07, face.y * 0.36)
        + vec2(18.4, -2.7));
    verticalJoint *= smoothstep(-0.28, 0.13, jointBreak + projectedMacro * 0.35);
    float rockVisibility = 1.0 - smoothstep(0.18, 0.50, facePixelWidth * 0.90);
    float rockGrain = valueNoise(vec2(faceAcross, face.y) * vec2(0.90, 1.05)
        + vec2(31.8, -8.4)) * rockVisibility;
    float fractureField = valueNoise(vec2(faceAcross * 0.19,
        face.y * 0.23) + vec2(4.2, 9.3));
    float fractureWidth = max(0.014, facePixelWidth * 0.19);
    float fracture = 1.0 - smoothstep(0.018, 0.018 + fractureWidth, abs(fractureField));
    weatheredRock *= 1.0 + rockGrain * 0.24 + projectedMottle * 0.18
        + bedding * 0.055 - beddingSeam * 0.085 - verticalJoint * 0.075
        - fracture * 0.06 * (1.0 - smoothstep(0.10, 0.35, facePixelWidth * 0.19));
    return RockFaceMaterial(weatheredRock, outcropField, cliffMacroModulation);
}
)";
}
