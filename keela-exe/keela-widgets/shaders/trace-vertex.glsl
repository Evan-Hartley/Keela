#version 330

/*
A point to plot in non-clip coordinates
*/
layout (location = 0) in float y;
uniform uint numSamples;
uniform bool invertY = false;

uniform float sampleMax = 255;
uniform float sampleMin = 0;

uniform float xoffset = 0;

void main() {
    float x = gl_VertexID;
    float max = sampleMax;
    float min = sampleMin;
    
    // try to vertically center the plot if max == min
    if (max == min) {
        max += 0.1;
        min -= 0.1;
    }

    float span = max - min;
    float scale = 1 / span;
    float yoffset = 0;

    // offset is applied to every y coord such that sampleMin will be transformed to 0.0
    yoffset = min >= 0 ? min : -min;
    gl_Position = vec4(((x + xoffset) * 2 / numSamples) - 1, 2 * scale * (y - yoffset) - 1, 0.0, 1.0);
    
    if (invertY) {
        gl_Position.y = -gl_Position.y;
    }
}