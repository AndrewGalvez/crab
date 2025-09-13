#version 330 core
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;
#define TEXTURE2D texture
#define OUTPUT_COLOR finalColor

uniform sampler2D texture0;

void main() {
   float alpha = TEXTURE2D(texture0, fragTexCoord).a;
   OUTPUT_COLOR = vec4(1.0, 1.0, 1.0, alpha);
}
