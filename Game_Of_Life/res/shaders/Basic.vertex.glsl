#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 u_MVP;
uniform uint u_state;

out float pixColor;

void main()
{
	vec4 newPosition = position;
	newPosition.y = position.y - gl_InstanceID;
	gl_Position = u_MVP * newPosition;

	pixColor = (u_state & (uint(1) << (7 - (gl_VertexID / 4)))) > uint(0) ? 1.0 : 0.0;
};
