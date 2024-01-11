#version 450 core
layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 oFrag;
layout(set = 0, binding = 0) uniform sampler2D tex00[1];
void main()
{
    oFrag = texture(tex00[0], vTexCoord);
}
