#version 330 core
layout (location = 0) in vec3 a_vertexPos;
layout (location = 1) in float a_vertexColor;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

out vec4 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    if (a_vertexColor > 0.0009f)
    {
        VertexColor = vec4(0.8, 0.0, 0.0, 0.0);
    }
    else
    {
        VertexColor = vec4(0.0, 0.0, 0.5, 0.0);
    }

//    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(a_vertexPos, 1.0);
}