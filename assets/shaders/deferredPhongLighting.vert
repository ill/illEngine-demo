//TODO: reorganize this later to have separate files for lights and stuff

uniform mat4 modelView;
uniform mat4 modelViewProjection;

varying vec3 viewPosition;

void main(void) {     
    //position in view space
    viewPosition = (modelView * gl_Vertex).xyz;
   
    //position on screen
    gl_Position = modelViewProjection * gl_Vertex;
}
