#version 330

// Input
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

uniform int id;
uniform int idSelected;
uniform int isColliding;

void main()
{
    if(isColliding == 1) {out_color = vec4(0.3, 1, 1, 1);}// * vec4(frag_color, 1);}
    else {
        if(id == idSelected)
            out_color = vec4(1, 0, 0, 1);// * vec4(frag_color, 1);
        else
            out_color = vec4(frag_color, 1);
        }
    out_color.a = id / 255.0f;
}
