#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_loc;

uniform mat3 u_transform;

out vec2 v_position;

void main()
{
    vec3 pos = u_transform * vec3(a_pos, 1.0f);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    v_position = a_loc;
}
