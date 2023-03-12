#version 330

uniform vec4 color;
uniform sampler2D tex;

in vec2 texCoordFrag;
in vec2 texCoord2Frag;
out vec4 outColor;

void main()
{
	vec4 texColor = texture(tex, texCoordFrag);
	if (texCoordFrag.x >= 0.0f && texColor.a >= 0.5f)
	{
		outColor = color * texColor;
	}
	else 
	{
		outColor = color * texture(tex, texCoord2Frag);
	}
}

