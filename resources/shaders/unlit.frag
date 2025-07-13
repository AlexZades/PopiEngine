#version 450

in vec2 inTexCoords;
uniform sampler2D DIFFUSE0;
layout (location = 0) out vec4 outColor;

void main() {

    outColor = texture(DIFFUSE0, inTexCoords);
}
