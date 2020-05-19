
#ifdef __COMPILE_VERTEX__

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 TexCoord;
out vec4 ourColor;

uniform vec4 baseColor;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model_matrix * vec4(position, 1.0);
	TexCoord = texCoord;
	ourColor = baseColor;
}
#endif

//---------------------------------------------------------------------------

#ifdef __COMPILE_FRAGMENT__

in vec2 TexCoord;
in vec4 ourColor;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	color = texture(ourTexture, TexCoord) * ourColor;
}
#endif