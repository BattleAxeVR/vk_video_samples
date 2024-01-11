#version 450 core
layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec2 aTexCoord;
layout(location = 0) out vec2 vTexCoord;

layout( push_constant ) uniform constants
{
    mat4 posMatrix;
    mat2 texMatrix;
} transformPushConstants;

void main()
{
    vTexCoord = transformPushConstants.texMatrix * aTexCoord;
    gl_Position = vec4(aVertex, 0, 1);
}
