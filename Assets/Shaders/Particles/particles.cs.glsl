#version 430

layout (local_size_x = 1000) in;

// black hole #1
uniform float gravity1 = 1000.0;
uniform vec3 blackHole1Pos = vec3 (5.0, 0.0, 0.0);

// block hole #2
uniform float gravity2 = 1000.0;
uniform vec3 blackHole2Pos = vec3 (-5.0, 0.0, 0.0);

// particles
uniform float particleInvMass = 1.0 / 0.1;
uniform float deltaTime = 0.0005;
uniform float maxDist = 45.0;

// out
layout (std430, binding = 0) buffer pos {
    vec4 position[];
};
layout (std430, binding = 1) buffer vel {
    vec4 velocity[];
};

void main (void) {

    const uint idx = gl_GlobalInvocationID.x;
    const vec3 p = position[idx].xyz;

    // Force from block hole #1.
    const vec3 delta1 = blackHole1Pos - p;
    const float dist1 = length (delta1);
    const vec3 force1 = (gravity1 / dist1) * normalize (delta1);

    // Force from block hole #2.
    const vec3 delta2 = blackHole2Pos - p;
    const float dist2 = length (delta2);
    const vec3 force2 = (gravity2 / dist2) * normalize (delta2);

    // Reset particles that get too far from the hole.
    if (dist1 > maxDist) {
        position[idx] = vec4 (0.0, 0.0, 0.0, 1.0);
    }
    // Apply euler intergrator.
    else {
        const vec3 force = force1 + force2;
        const vec3 acceleration = force * particleInvMass;
        position[idx] = vec4 (p + velocity[idx].xyz * deltaTime + 0.5 * acceleration * deltaTime * deltaTime, 1.0);
        velocity[idx] = vec4 (velocity[idx].xyz + acceleration * deltaTime, 0.0);
    }
}
