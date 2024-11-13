#version 450 core

layout(location = 0) in vec2 v_TexCoord;

uniform sampler2D uTexture;

layout(location = 0) out vec4 o_Color;

void main()
{
    vec4 texColor = texture(uTexture, v_TexCoord);

    if (texColor.a == 0.0)
        discard;

    o_Color = texColor;
}
