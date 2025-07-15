#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;
uniform vec2 resolution;

// Output fragment color
out vec4 finalColor;

// Pseudo-random function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Block-based random function
float blockRandom(vec2 st, float blockSize) {
    vec2 blockPos = floor(st * blockSize);
    return random(blockPos + 0.1 * time);
}

void main() {
    // Get the original texture color
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Calculate actual screen coordinates
    vec2 screenCoord = gl_FragCoord.xy;
    
    // Block size (adjust this to change grain size)
    float blockSize = 0.1;
    
    // Generate blocky noise
    float noise = blockRandom(screenCoord, blockSize);
    
    // Create grain intensity (0.0 to 1.0)
    float grainIntensity = 0.3;
    
    // Apply grain effect
    float grain = (noise - 0.5) * grainIntensity;
    
    // Calculate luminance to determine if pixel is dark
    float luminance = dot(texelColor.rgb, vec3(0.299, 0.587, 0.114));
    
    vec3 grainedColor;
    
    // For very dark pixels (including black), add positive grain
    if (luminance < 0.05) {
        grainedColor = texelColor.rgb + abs(grain);
    } else {
        // For other pixels, use normal grain (can be positive or negative)
        grainedColor = texelColor.rgb + grain;
    }
    
    // Ensure we don't exceed color bounds
    grainedColor = clamp(grainedColor, 0.0, 1.0);
    
    // Output final color
    finalColor = vec4(grainedColor, texelColor.a) * colDiffuse * fragColor;
}
