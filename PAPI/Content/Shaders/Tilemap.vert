#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 uViewProjection;
uniform vec3 uPos;
uniform ivec2 uChunkSize;
uniform sampler2D uTexture;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    vec2 coord = vec2(gl_InstanceID % uChunkSize.x, gl_InstanceID / uChunkSize.x);
    vec3 pos = uPos + vec3(coord, 0.0) + a_Position;
    gl_Position = uViewProjection * vec4(pos, 1.0);
    v_TexCoord = a_TexCoord;
}
