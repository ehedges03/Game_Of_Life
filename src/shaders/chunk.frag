#version 330 core

in vec2 v_position;

uniform uint u_data[2];

out vec4 FragColor;

void main()
{
    vec2 calced = v_position * 8;
    calced = floor(calced);
    int index = int(calced.x) + int(calced.y) * 8;
    uint value = (u_data[index / 32] >> 31 - (index % 32)) & 1u;
    FragColor = vec4(float(value), float(value), float(value), 1.0f);
}
