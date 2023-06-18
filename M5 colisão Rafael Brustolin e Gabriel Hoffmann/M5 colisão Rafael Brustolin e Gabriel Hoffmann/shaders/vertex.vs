#version 450 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texture_mapping;

out vec2 texture_coords;
  
uniform mat4 model;
uniform mat4 projection;

void main()
{
	texture_coords = texture_mapping;
    gl_Position = projection * model * vec4(position, 1.0f);
}