#version 450

in vec2 inTexCoords;
uniform sampler2D DIFFUSE0;
uniform vec3 DIFFUSE_COLOR;
layout (location = 0) out vec4 outColor;

void main() {

    outColor = texture(DIFFUSE0, inTexCoords) * vec4(DIFFUSE_COLOR,1.0f);
}
