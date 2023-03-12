#version 330

uniform mat4 projection, modelview;
uniform vec2 texCoordDispl;

in vec2 position;
in vec2 texCoord;
in vec2 texCoord2; // background
out vec2 texCoordFrag;
out vec2 texCoord2Frag;

void main()
{
	// Pass texture coordinates to access a given texture atlas
	texCoordFrag = texCoord + texCoordDispl;
	texCoord2Frag = texCoord2 + texCoordDispl;
	// Transform position from pixel coordinates to clipping coordinates
	gl_Position = projection * modelview * vec4(position, 0.0, 1.0);
}

