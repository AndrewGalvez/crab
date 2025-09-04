// white_mask_webgl.fs
#version 300 es
precision mediump float;

in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
out vec4 finalColor;

void main() {
    float alpha = texture(texture0, fragTexCoord).a;
    finalColor = vec4(1.0, 1.0, 1.0, alpha);
}
