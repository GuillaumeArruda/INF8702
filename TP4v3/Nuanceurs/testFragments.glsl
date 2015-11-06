uniform sampler2D colorMap;

void main (void) 
{
    vec4 color = texture2D(colorMap, gl_TexCoord[0].xy);
    gl_FragColor = color;
}