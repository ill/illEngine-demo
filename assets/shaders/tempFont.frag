varying vec2 texCoordsOut;
uniform sampler2D diffuseMap;
uniform vec4 color;

void main() {
	gl_FragColor = texture2D(diffuseMap, texCoordsOut) * color;
}