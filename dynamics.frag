#version 330 core

layout(location = 0) out vec3 density_out;
layout(location = 1) out vec3 velocity_out;

uniform sampler2D density;
uniform sampler2D velocity;

uniform int w, h;
uniform int t;

float to_density(vec3 d) {
  return d.x + d.y * 255 + d.z * 256 * 255;
}

vec3 to_vec(float d) {
  float x = d - int(d);
  d = (d - x) / 255;
  float y = d - int(d);
  d = (d - y) / 255;
  float z = d - int(d);
  return vec3(x,y,z);
}

// This is to calculate transfer to neighbors
// such that the transfer can be calculated
// independently for each neighbor, yet sum to 1.
// Assumes length(v) <= 1
float box_transfer(int dx, int dy, vec2 v) {
  if (dx != 0 && dx != sign(v.x) ||
      dy != 0 && dy != sign(v.y))
    return 0; // Boxes cannot overlap.

  if (dx == 0) {
    if (dy == 0) {
      return (1.0 - abs(v.x)) * (1.0 - abs(v.y));
    } else {
      return (1.0 - abs(v.x)) * abs(v.y);
    }
  } else {
    if (dy == 0) {
      return abs(v.x) * (1.0 - abs(v.y));
    } else {
      return abs(v.x) * abs(v.y);
    }
  }
}

vec2 mass_from(vec2 v1, float d1, vec2 v2, float d2, vec2 dir) {
  float mot = 0;
  float diff = 0;

  // Motion
  float transfer = box_transfer(int(sign(dir.x)), int(sign(dir.y)), v1);
  if (transfer > 0) {
    mot += transfer * d1;
  }

  // Diffusion
  if (d1 > d2) {
    diff += (d1 - d2) * .1;
  }

  return vec2(mot, diff);
}

void main() {
     vec2 pos = vec2(float(gl_FragCoord.x) / w, float(gl_FragCoord.y) / h);
     float density_in = to_density(texture2D(density, pos).rgb);
     vec3 velocity_in = texture2D(velocity, pos).rgb - vec3(.5,.5,.5);

     vec2 scale = vec2(1.0 / w, 1.0 / h);
     float density_res = density_in;
     velocity_out = velocity_in * density_in;

     /*
      * 3 steps:
      *   1. Mass transfer
      *   2. Diffusion (folded into 1)
      *   3. Determine output velocity
      */

     // For step 3.
     vec3 res_v = vec3(0,0,0);
     float total_mass = 0;

     for (int dx = -1; dx <= 1; dx++) {
     	 for (int dy = -1; dy <= 1; dy++) {
	     if (dx == 0 && dy == 0)
	     	continue;
     	     vec2 dir = vec2(dy, dx);
     	     vec2 dir_scaled = dir * scale;

	     float density_loc = to_density(texture2D(density, pos + dir_scaled).rgb);
	     vec3 velocity_loc = texture2D(velocity, pos + dir_scaled).rgb - vec3(.5,.5,.5);

	     /*
	     if (pos.x + dir_scaled.x <= 0 || pos.x + dir_scaled.x >= 1) {
	       density_loc = density_in;
	       if (pos.y + dir_scaled.y <= 0 || pos.y + dir_scaled.y >= 1) {
		 velocity_loc = -velocity_in;
	       } else {
		 velocity_loc = vec3(-velocity_in.x, velocity_in.y, 0);
	       }
	     } else if (pos.y + dir_scaled.y <= 0 || pos.y + dir_scaled.y >= 1) {
	       density_loc = density_in;
	       velocity_loc = vec3(velocity_in.x, -velocity_in.y, 0);
	     }
	     */

	     dir = normalize(dir);

	     res_v += velocity_loc * density_loc;
	     total_mass += density_loc;

	     vec2 mass_in = mass_from(velocity_loc.rg, density_loc, velocity_in.rg, density_in, -dir);
	     vec2 mass_out = mass_from(velocity_in.rg, density_in, velocity_loc.rg, density_loc, dir);

	     density_res += mass_in.x + mass_in.y - mass_out.x - mass_out.y;

	     velocity_out += mass_in.x * velocity_loc + mass_in.y * (.5 * velocity_loc - .2 * vec3(dir,0)) -
	       mass_out.x * velocity_in - mass_out.y * (.5 * velocity_in + .2 * vec3(dir,0));

	       /*
	     float veldir = .9;//dot(velocity_loc.xy, normalize(dir));
	     float odir = .1;//sqrt(1 - veldir * veldir);
	     velocity_out += mass_in * (velocity_loc * veldir - vec3(dir,0) * odir) -
	       mass_out * (velocity_in * veldir + vec3(dir,0)* odir);
	       */
	 }
     }
     /*
     velocity_out += res_v * .02;
     total_mass = total_mass * .02 + density_res;
     velocity_out /= total_mass;
     */
     
     velocity_out /= density_res;

     // source.
     if (length(pos.xy - vec2(.5,.5)) < .03) {
       float time = float(t) / 1000;
       velocity_out += vec3(cos(time), sin(time),0) * .01;
       density_res += .03;
     }

     // damping.
     // velocity_out *= .999; // This is *way* too severe...
     velocity_out += vec3(.5, .5, .5);
     if (length(velocity_out) > 1)
       velocity_out = normalize(velocity_out);


     density_out = to_vec(density_res);
}
