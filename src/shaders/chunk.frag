#version 330 core
out vec4 FragColor;
in vec3 ourColor;

uniform uint uData[2];

void main()
{
    vec3 calced = ourColor * 8;
    calced = floor(calced);
    int index = int(calced.x) + int(calced.y) * 8;
    uint value = (uData[index / 32] >> 31 - (index % 32)) & 1u;
    FragColor = vec4(float(value), float(value), float(value), 1.0f);
}
