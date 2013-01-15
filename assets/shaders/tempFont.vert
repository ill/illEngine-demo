attribute vec4 position;
attribute vec2 texCoords;
uniform mat4 modelViewProjectionMatrix;

varying vec2 texCoordsOut;

void main()
{
	texCoordsOut = texCoords;
    gl_Position = modelViewProjectionMatrix * position;
}