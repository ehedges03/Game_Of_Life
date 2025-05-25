#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aLoc;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    ourColor = vec3(aLoc, 1.0f);
}
