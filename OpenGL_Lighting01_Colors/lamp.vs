#version 330 core
layout ( location = 0 ) in vec3 position;


uniform mat4 model;			// Converts local object coordinates to Camera coordinates
uniform mat4 projection;	// Converts Camera coordinates to normalized coordinates (means coordinates between 0 and 1)
uniform mat4 view;			// Converts the the normalized to Window coordinates (aka your screen, your window )


void main ()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	
}

