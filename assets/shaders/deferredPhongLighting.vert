//TODO: reorganize this later to have separate files for lights and stuff

uniform mat4 modelView;
uniform mat4 modelViewProjection;

varying vec3 viewPosition;
attribute vec3 positionIn;

void main(void) {     
    //position in view space
    viewPosition = (modelView * vec4(positionIn, 1.0)).xyz;
   
    //position on screen
    gl_Position = modelViewProjection * vec4(positionIn, 1.0);
}
