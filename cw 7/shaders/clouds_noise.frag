#version 430 core

uniform float u_time;
out vec4 outColor;
in vec2 vtc;
in vec3 worldPos;

vec2 u_resolution = vec2(1024, 1024);

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

void main() {
    float cloudIntensity = 15.0;

    vec2 st = vtc.xy;
    vec2 mirroredSt = vec2(1.0 - st.x, st.y);

    // Adjust animation to be boundary-aware
    float timeOffset = u_time * 0.07;
    st.x -= timeOffset;
    mirroredSt.x += timeOffset; // Inverse direction for mirrored effect

    // Ensure wrapping or clamping based on your texture coordinates' expectations
    st.x = fract(st.x);
    mirroredSt.x = fract(mirroredSt.x);

    // Calculate cloud patterns
    vec3 color = vec3(fbm(st * cloudIntensity));
    vec3 mirroredColor = vec3(fbm(mirroredSt * cloudIntensity));

    // Dynamic gradient based on X coordinate, ensuring smooth transition
    float blend = smoothstep(0.45, 0.55, st.x); // Adjust these values as needed

    // Blend based on the dynamic gradient
    vec3 finalColor = mix(color, mirroredColor, blend);

    outColor = vec4(finalColor, 1.0);
}
