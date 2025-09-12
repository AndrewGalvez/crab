#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;
uniform sampler2D texture0;

void main() {
    float alpha = texture2D(texture0, fragTexCoord).a;
    gl_FragColor = vec4(1.0, 1.0, 1.0, alpha);
}

