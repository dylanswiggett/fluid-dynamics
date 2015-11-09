#version 330 core

layout(location = 0) out vec3 color;

uniform sampler2D density;
uniform sampler2D velocity;

uniform int w;
uniform int h;

void main() {
     vec2 pos = vec2(float(gl_FragCoord.x) / w, float(gl_FragCoord.y) / h);
     vec3 density_in = texture2D(density, pos).rgb;
     vec3 velocity_in = texture2D(velocity, pos).rgb - vec3(.5,.5,.5);

     color = velocity_in * dot(density_in, vec3(1,1,1));
     return;

     color = density_in;// + velocity_in;
     color += vec3(0,1,0) * length(velocity_in) * dot(density_in, vec3(1,1,1));
}