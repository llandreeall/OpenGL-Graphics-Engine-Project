#version 330

// Input
in vec3 frag_color;
in vec2 tex_coord;

uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    vec4 color1 = texture(texture_1, tex_coord);  
    if(color1.w < 0.1f)
        discard;
    out_color = color1;//vec4(frag_color, 1);
}
