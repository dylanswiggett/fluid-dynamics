#version 330 core

layout(location = 0) out vec3 density_out;
layout(location = 1) out vec3 velocity_out;

uniform sampler2D density;
uniform sampler2D velocity;

uniform int w, h;

float motion_from(vec2 src, vec2 dst) {

     vec2 dir = dst - src;
     vec2 dir_normalized = normalize(dir);

     vec3 density_src = texture2D(density, src).xyz;
     vec2 velocity_src = texture2D(velocity, src).xy - vec2(.5, .5);
     vec3 density_dst = texture2D(density, dst).xyz;
     vec2 velocity_dst = texture2D(velocity, dst).xy - vec2(.5, .5);

     // Transfer from liquid motion
     float v_transfer = dot(velocity_src, dir_normalized) / 8;
     v_transfer = clamp(v_transfer, 0, 1);
     
     // Transfer from diffusion
     float total_d_src = dot(density_src, vec3(1,1,1));
     float total_d_dst = dot(density_dst, vec3(1,1,1));
     float d_transfer = 0;
     if (total_d_dst < total_d_src) {
     	d_transfer = 1 - (total_d_dst / total_d_src);
	d_transfer /= 8;
     }

     return v_transfer + d_transfer;
}

float mass_from(vec2 v1, float d1, vec2 v2, float d2, vec2 dir) {
  dir = normalize(dir);

  float mass = 0;

  // Motion
  float transfer = dot(v1, dir);
  if (transfer > 0) {
    mass += transfer * d1;
  }

  // Diffusion
  if (d1 > d2) {
    mass += (d1 - d2) * .1;
  }

  return mass;
}

void main() {
     vec2 pos = vec2(float(gl_FragCoord.x) / w, float(gl_FragCoord.y) / h);
     vec3 density_in = texture2D(density, pos).rgb;
     vec3 velocity_in = texture2D(velocity, pos).rgb - vec3(.5,.5,.5);

     vec2 scale = vec2(1.0 / w, 1.0 / h);
     density_out = density_in;
     velocity_out = velocity_in * dot(density_in, vec3(1,1,1));

     /*
      * 3 steps:
      *   1. Mass transfer
      *   2. Diffusion (folded into 1)
      *   3. Determine output velocity
      */

     // For step 3.
     vec3 res_v;
     float total_mass = 0;

     for (int dx = -1; dx <= 1; dx++) {
     	 for (int dy = -1; dy <= 1; dy++) {
	     if (dx == 0 && dy == 0)
	     	continue;
	 
     	     vec2 dir = vec2(dy, dx);
     	     vec2 dir_scaled = dir * scale;
		
	     vec3 density_loc = texture2D(density, pos + dir_scaled).rgb;
	     vec3 velocity_loc = texture2D(velocity, pos + dir_scaled).rgb - vec3(.5,.5,.5);

	     float transfer_in = motion_from(pos + dir_scaled, pos);
	     float transfer_out = motion_from(pos, pos + dir_scaled);

	     vec3 add_density = density_loc * transfer_in;
	     vec3 lose_density = density_in * transfer_out;
	     density_out += add_density - lose_density;
	     velocity_out += vec3(dir,0) * dot(add_density, vec3(1,1,1));
	     velocity_out -= vec3(dir,0) * dot(lose_density, vec3(1,1,1));
	 }
     }

     velocity_out /= dot(density_out, vec3(1,1,1));

     // damping.
     velocity_out *= .99;
     velocity_out += vec3(.5, .5, .5);
     velocity_out = clamp(velocity_out, 0, 1);


     // source.
     if (pos.x > .47 && pos.x < .53 && pos.y > .47 && pos.y < .53) {
     	//velocity_out = vec3(1,0,0);
	density_out += vec3(.1,0,0);
     }

}
