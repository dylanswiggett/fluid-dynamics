#version 330 core

layout(location = 0) out vec3 color;

uniform sampler2D density;
uniform sampler2D velocity;

uniform int w;
uniform int h;

float to_density(vec3 d) {
  return d.x + d.y * 255 + d.z * 256 * 255;
}

void main() {
  color = vec3(0,0,0);

  for (int dx = -1; dx <= 1; dx++) {
    for (int dy = -1; dy <= 1; dy++) {
      vec2 pos = vec2(float(gl_FragCoord.x + dx * 3) / w, float(gl_FragCoord.y + dy * 3) / h);
      float density_in = to_density(texture2D(density, pos).rgb);
      density_in = clamp(density_in, 0,5);
      vec3 velocity_in = texture2D(velocity, pos).rgb - vec3(.5,.5,.5);

      color += vec3(1,1,1) * density_in / 5;
      //color += vec3(0,1,0) * length(velocity_in) * density_in;
    }
  }

  color /= 9;

  /*
  vec2 pos = vec2(float(gl_FragCoord.x) / w, float(gl_FragCoord.y) / h);
  float density_in = texture2D(density, pos).r;
  vec3 velocity_in = texture2D(velocity, pos).rgb - vec3(.5,.5,.5);

  //color = vec3(1,0,0) * density_in;
  //return;

  color = vec3(density_in,0,0);// + velocity_in;
  color += vec3(0,1,0) * length(velocity_in) * density_in;
  */
}
