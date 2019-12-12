#version 330 core

#ifdef __COMPILE_VERTEX__

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
	TexCoord = texCoord;
}

#endif

//---------------------------------------------------------------------------

#ifdef __COMPILE_FRAGMENT__

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
	color = texture(ourTexture, TexCoord);
}

#endif