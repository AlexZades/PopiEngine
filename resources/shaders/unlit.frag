#version 450
//My 4x4 bayer luma dither shader
//Based on https://alex-charlton.com/posts/Dithering_on_the_GPU/
//and https://gist.github.com/anaya92/dd5a4fbc8d3ddb8ceb604210b502ab52
in vec2 inTexCoords;
uniform sampler2D DIFFUSE0;
uniform vec3 DIFFUSE_COLOR;
uniform float TIME;
layout (location = 0) out vec4 outColor;
float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587, 0.114));
}

float luma(vec4 color) {
  return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}

const int bayerMatrix8[64] = int[](0,  32, 8,  40, 2,  34, 10, 42,
                                     48, 16, 56, 24, 50, 18, 58, 26,
                                     12, 44, 4,  36, 14, 46, 6,  38,
                                     60, 28, 52, 20, 62, 30, 54, 22,
                                     3,  35, 11, 43, 1,  33, 9,  41,
                                     51, 19, 59, 27, 49, 17, 57, 25,
                                     15, 47, 7,  39, 13, 45, 5,  37,
                                     63, 31, 55, 23, 61, 29, 53, 21);
const int bayerMatrix4[16] = int[16](0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);
float bayerLimit(int index){
    
    return float(bayerMatrix8[index])/ 64.0;
}

float dither(vec2 position, float brightness) {

  int x = int(mod(position.x + (TIME/100), 8.0));
  int y = int(mod(position.y - 2*cos(TIME/500 -0.5), 8.0));

  int index = x + y * 8;
  float limit = 0.0;

  if (x < 8) {
    limit = bayerLimit(index);
  }

  return brightness < limit ? 0.0 : 1.0;
}

vec3 dither(vec2 position, vec3 color) {
  return color * dither(position, luma(color));
}
void main() {
    
    vec4 diffuse = texture(DIFFUSE0, inTexCoords) * vec4(DIFFUSE_COLOR,1.0f);
    outColor = vec4(dither(gl_FragCoord.xy, diffuse.rgb),diffuse.a);
}
