#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
out vec3 ourPos;

uniform float xOffset;

void main()
{
    gl_Position = vec4(aPos.x-0.5+xOffset, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    ourPos = aPos;
}