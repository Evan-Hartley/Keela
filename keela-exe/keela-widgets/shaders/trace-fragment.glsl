#version 330

out vec4 FragColor;
void main() {
    // #BA7D71 (in memory of kevin from louisville kentucky (he's still alive))
    vec3 rgb = vec3(186, 125, 113);
    FragColor = vec4(rgb / 255, 1.0);
}