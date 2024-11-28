#version 450 core

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat int a_FontAtlas;

layout (binding = 0) uniform sampler2D u_Textures[32];

const float pixelRange = 2.0f;

layout (location = 0) out vec4 o_Color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
    vec2 unitRange = vec2(pixelRange)/vec2(textureSize(u_Textures[a_FontAtlas], 0)); // MW @todo: Shouldn't be indexing with variable.
    vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    vec4 texColor = vec4(1);

    switch (int(a_FontAtlas))
    {
        case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord); break;
        case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord); break;
        case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord); break;
        case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord); break;
        case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord); break;
        case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord); break;
        case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord); break;
        case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord); break;
        case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord); break;
        case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord); break;
        case 10: texColor *= texture(u_Textures[10], Input.TexCoord); break;
        case 11: texColor *= texture(u_Textures[11], Input.TexCoord); break;
        case 12: texColor *= texture(u_Textures[12], Input.TexCoord); break;
        case 13: texColor *= texture(u_Textures[13], Input.TexCoord); break;
        case 14: texColor *= texture(u_Textures[14], Input.TexCoord); break;
        case 15: texColor *= texture(u_Textures[15], Input.TexCoord); break;
        case 16: texColor *= texture(u_Textures[16], Input.TexCoord); break;
        case 17: texColor *= texture(u_Textures[17], Input.TexCoord); break;
        case 18: texColor *= texture(u_Textures[18], Input.TexCoord); break;
        case 19: texColor *= texture(u_Textures[19], Input.TexCoord); break;
        case 20: texColor *= texture(u_Textures[20], Input.TexCoord); break;
        case 21: texColor *= texture(u_Textures[21], Input.TexCoord); break;
        case 22: texColor *= texture(u_Textures[22], Input.TexCoord); break;
        case 23: texColor *= texture(u_Textures[23], Input.TexCoord); break;
        case 24: texColor *= texture(u_Textures[24], Input.TexCoord); break;
        case 25: texColor *= texture(u_Textures[25], Input.TexCoord); break;
        case 26: texColor *= texture(u_Textures[26], Input.TexCoord); break;
        case 27: texColor *= texture(u_Textures[27], Input.TexCoord); break;
        case 28: texColor *= texture(u_Textures[28], Input.TexCoord); break;
        case 29: texColor *= texture(u_Textures[29], Input.TexCoord); break;
        case 30: texColor *= texture(u_Textures[30], Input.TexCoord); break;
        case 31: texColor *= texture(u_Textures[31], Input.TexCoord); break;
    }
    
    vec3 msd = texColor.rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    o_Color = Input.Color;
    o_Color.a = opacity;
}
