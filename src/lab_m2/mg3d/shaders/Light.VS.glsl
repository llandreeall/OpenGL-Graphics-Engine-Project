#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 light_pos;
uniform vec3 eye_pos;
uniform float mat_kd;
uniform float mat_ks;
uniform int mat_shininess;

uniform vec3 color;

// Output

out vec3 frag_color;



void main()
{
    vec3 world_pos = (Model * vec4(v_position,1)).xyz;
    vec3 world_norm = normalize( mat3(Model) * v_normal );

    vec3 L = normalize(light_pos - world_pos);
    vec3 V = normalize(eye_pos - world_pos);
    vec3 H = normalize(L + V);

    float ambient_light = 0.25 *  mat_kd;
    float diffuse_light = 0;
    diffuse_light = mat_kd * max(dot(normalize(world_norm),L),0);

    float specular_light = 0;
    vec3 R = reflect(-L, normalize(world_norm));

    if (diffuse_light > 0)
    {
        specular_light = mat_ks * max(dot(normalize(world_norm), L),0) * pow(max(dot(normalize(world_norm), H), 0), mat_shininess);
    } 

    float d = distance(light_pos,world_pos);
    float f_at = 1/pow(d,2);
    float light = ambient_light + f_at * ( diffuse_light + specular_light );
    
    frag_color = color * light;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
