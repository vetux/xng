/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_HLSL_NOISE_HPP
#define XENGINE_HLSL_NOISE_HPP

static const char *HLSL_NOISE = R"###(
// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
// Ported to HLSL

#include "pi.hlsl"

float permute(float x){return floor(modf(((x*34.0)+1.0)*x, 289.0));}
float3 permute(float3 x) { return modf(((x*34.0)+1.0)*x, 289.0); }
float4 permute(float4 x){return modf(((x*34.0)+1.0)*x, 289.0);}

float taylorInvSqrt(float r){return 1.79284291400159 - 0.85373472095314 * r;}
float4 taylorInvSqrt(float4 r){return 1.79284291400159 - 0.85373472095314 * r;}

// Simplex Noise

// Simplex 2D noise

float simplex(float2 v){
  const float4 C = float4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  float2 i  = floor(v + dot(v, C.yy) );
  float2 x0 = v -   i + dot(i, C.xx);
  float2 i1;
  i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
  float4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = modf(i, 289.0);
  float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 ))
  + i.x + float3(0.0, i1.x, 1.0 ));
  float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  float3 x = 2.0 * frac(p * C.www) - 1.0;
  float3 h = abs(x) - 0.5;
  float3 ox = floor(x + 0.5);
  float3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  float3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts

float simplex(float3 v){
  const float2  C = float2(1.0/6.0, 1.0/3.0) ;
  const float4  D = float4(0.0, 0.5, 1.0, 2.0);

  // First corner
  float3 i  = floor(v + dot(v, C.yyy) );
  float3 x0 =   v - i + dot(i, C.xxx) ;

  // Other corners
  float3 g = step(x0.yzx, x0.xyz);
  float3 l = 1.0 - g;
  float3 i1 = min( g.xyz, l.zxy );
  float3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  float3 x1 = x0 - i1 + 1.0 * C.xxx;
  float3 x2 = x0 - i2 + 2.0 * C.xxx;
  float3 x3 = x0 - 1. + 3.0 * C.xxx;

  // Permutations
  i = modf(i, 289.0 );
  float4 p = permute( permute( permute(
             i.z + float4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + float4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + float4(0.0, i1.x, i2.x, 1.0 ));

  // Gradients
  // ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  float3  ns = n_ * D.wyz - D.xzx;

  float4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  modf(p,N*N)

  float4 x_ = floor(j * ns.z);
  float4 y_ = floor(j - 7.0 * x_ );    // modf(j,N)

  float4 x = x_ *ns.x + ns.yyyy;
  float4 y = y_ *ns.x + ns.yyyy;
  float4 h = 1.0 - abs(x) - abs(y);

  float4 b0 = float4( x.xy, y.xy );
  float4 b1 = float4( x.zw, y.zw );

  float4 s0 = floor(b0)*2.0 + 1.0;
  float4 s1 = floor(b1)*2.0 + 1.0;
  float4 sh = -step(h, float4(0.0));

  float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  float3 p0 = float3(a0.xy,h.x);
  float3 p1 = float3(a0.zw,h.y);
  float3 p2 = float3(a1.xy,h.z);
  float3 p3 = float3(a1.zw,h.w);

  //Normalise gradients
  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  // Mix final noise value
  float4 m = max(0.6 - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, float4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}


//	Simplex 4D Noise
//	by Ian McEwan, Ashima Arts

float4 grad4(float j, float4 ip){
  const float4 ones = float4(1.0, 1.0, 1.0, -1.0);
  float4 p,s;

  p.xyz = floor( frac (float3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
  p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
  s = float4(lessThan(p, float4(0.0)));
  p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www;

  return p;
}

float simplex(float4 v){
  const float2  C = float2( 0.138196601125010504,  // (5 - sqrt(5))/20  G4
                        0.309016994374947451); // (sqrt(5) - 1)/4   F4
  // First corner
  float4 i  = floor(v + dot(v, C.yyyy) );
  float4 x0 = v -   i + dot(i, C.xxxx);

  // Other corners

  // Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
  float4 i0;

  float3 isX = step( x0.yzw, x0.xxx );
  float3 isYZ = step( x0.zww, x0.yyz );
  //  i0.x = dot( isX, float3( 1.0 ) );
  i0.x = isX.x + isX.y + isX.z;
  i0.yzw = 1.0 - isX;

  //  i0.y += dot( isYZ.xy, float2( 1.0 ) );
  i0.y += isYZ.x + isYZ.y;
  i0.zw += 1.0 - isYZ.xy;

  i0.z += isYZ.z;
  i0.w += 1.0 - isYZ.z;

  // i0 now contains the unique values 0,1,2,3 in each channel
  float4 i3 = clamp( i0, 0.0, 1.0 );
  float4 i2 = clamp( i0-1.0, 0.0, 1.0 );
  float4 i1 = clamp( i0-2.0, 0.0, 1.0 );

  //  x0 = x0 - 0.0 + 0.0 * C
  float4 x1 = x0 - i1 + 1.0 * C.xxxx;
  float4 x2 = x0 - i2 + 2.0 * C.xxxx;
  float4 x3 = x0 - i3 + 3.0 * C.xxxx;
  float4 x4 = x0 - 1.0 + 4.0 * C.xxxx;

  // Permutations
  i = modf(i, 289.0);
  float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
  float4 j1 = permute( permute( permute( permute (
             i.w + float4(i1.w, i2.w, i3.w, 1.0 ))
           + i.z + float4(i1.z, i2.z, i3.z, 1.0 ))
           + i.y + float4(i1.y, i2.y, i3.y, 1.0 ))
           + i.x + float4(i1.x, i2.x, i3.x, 1.0 ));
  // Gradients
  // ( 7*7*6 points uniformly over a cube, mapped onto a 4-octahedron.)
  // 7*7*6 = 294, which is close to the ring size 17*17 = 289.

  float4 ip = float4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;

  float4 p0 = grad4(j0,   ip);
  float4 p1 = grad4(j1.x, ip);
  float4 p2 = grad4(j1.y, ip);
  float4 p3 = grad4(j1.z, ip);
  float4 p4 = grad4(j1.w, ip);

  // Normalise gradients
  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;
  p4 *= taylorInvSqrt(dot(p4,p4));

  // Mix contributions from the five corners
  float3 m0 = max(0.6 - float3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
  float2 m1 = max(0.6 - float2(dot(x3,x3), dot(x4,x4)            ), 0.0);
  m0 = m0 * m0;
  m1 = m1 * m1;
  return 49.0 * ( dot(m0*m0, float3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
               + dot(m1*m1, float2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;
}

// Perlin Noise

float rand(float2 co){return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);}
float rand (float2 co, float l) {return rand(float2(rand(co), l));}
float rand (float2 co, float l, float t) {return rand(float2(rand(co, l), t));}

// Perlin with time scaling
float perlin(float2 p, float dim, float time) {
	float2 pos = floor(p * dim);
	float2 posx = pos + float2(1.0, 0.0);
	float2 posy = pos + float2(0.0, 1.0);
	float2 posxy = pos + float2(1.0);

	float c = rand(pos, dim, time);
	float cx = rand(posx, dim, time);
	float cy = rand(posy, dim, time);
	float cxy = rand(posxy, dim, time);

	float2 d = frac(p * dim);
	d = -0.5 * cos(d * PI) + 0.5;

	float ccx = lerp(c, cx, d.x);
	float cycxy = lerp(cy, cxy, d.x);
	float center = lerp(ccx, cycxy, d.y);

	return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(float2 p, float dim) {
	float2 pos = floor(p * dim);
	float2 posx = pos + float2(1.0, 0.0);
	float2 posy = pos + float2(0.0, 1.0);
	float2 posxy = pos + float2(1.0);

	float c = rand(pos, dim);
	float cx = rand(posx, dim);
	float cy = rand(posy, dim);
	float cxy = rand(posxy, dim);

	float2 d = frac(p * dim);
	d = -0.5 * cos(d * PI) + 0.5;

	float ccx = lerp(c, cx, d.x);
	float cycxy = lerp(cy, cxy, d.x);
	float center = lerp(ccx, cycxy, d.y);

	return center * 2.0 - 1.0;

	//return perlin(p, dim, 0.0);
}

// p must be normalized!
float perlinbw(float2 p, float dim) {
	float2 pos = floor(p * dim);
	float2 posx = pos + float2(1.0, 0.0);
	float2 posy = pos + float2(0.0, 1.0);
	float2 posxy = pos + float2(1.0);

	// For exclusively black/white noise
	float c = step(rand(pos, dim), 0.5);
	float cx = step(rand(posx, dim), 0.5);
	float cy = step(rand(posy, dim), 0.5);
	float cxy = step(rand(posxy, dim), 0.5);

	float2 d = frac(p * dim);
	d = -0.5 * cos(d * PI) + 0.5;

	float ccx = lerp(c, cx, d.x);
	float cycxy = lerp(cy, cxy, d.x);
	float center = lerp(ccx, cycxy, d.y);

	return center * 2.0 - 1.0;
}

// Classic Perlin Noise

//	Classic Perlin 2D Noise
//	by Stefan Gustavson

float2 fade(float2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
float3 fade(float3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
float4 fade(float4 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float perlinc(float2 P){
  float4 Pi = floor(P.xyxy) + float4(0.0, 0.0, 1.0, 1.0);
  float4 Pf = frac(P.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
  Pi = modf(Pi, 289.0); // To avoid truncation effects in permutation
  float4 ix = Pi.xzxz;
  float4 iy = Pi.yyww;
  float4 fx = Pf.xzxz;
  float4 fy = Pf.yyww;
  float4 i = permute(permute(ix) + iy);
  float4 gx = 2.0 * frac(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  float4 gy = abs(gx) - 0.5;
  float4 tx = floor(gx + 0.5);
  gx = gx - tx;
  float2 g00 = float2(gx.x,gy.x);
  float2 g10 = float2(gx.y,gy.y);
  float2 g01 = float2(gx.z,gy.z);
  float2 g11 = float2(gx.w,gy.w);
  float4 norm = 1.79284291400159 - 0.85373472095314 *
    float4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, float2(fx.x, fy.x));
  float n10 = dot(g10, float2(fx.y, fy.y));
  float n01 = dot(g01, float2(fx.z, fy.z));
  float n11 = dot(g11, float2(fx.w, fy.w));
  float2 fade_xy = fade(Pf.xy);
  float2 n_x = lerp(float2(n00, n01), float2(n10, n11), fade_xy.x);
  float n_xy = lerp(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

//	Classic Perlin 3D Noise
//	by Stefan Gustavson

float perlinc(float4 P){
  float4 Pi0 = floor(P); // Integer part for indexing
  float4 Pi1 = Pi0 + 1.0; // Integer part + 1
  Pi0 = modf(Pi0, 289.0);
  Pi1 = modf(Pi1, 289.0);
  float4 Pf0 = frac(P); // Fractional part for interpolation
  float4 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.yy, Pi1.yy);
  float4 iz0 = float4(Pi0.zzzz);
  float4 iz1 = float4(Pi1.zzzz);
  float4 iw0 = float4(Pi0.wwww);
  float4 iw1 = float4(Pi1.wwww);

  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);
  float4 ixy00 = permute(ixy0 + iw0);
  float4 ixy01 = permute(ixy0 + iw1);
  float4 ixy10 = permute(ixy1 + iw0);
  float4 ixy11 = permute(ixy1 + iw1);

  float4 gx00 = ixy00 / 7.0;
  float4 gy00 = floor(gx00) / 7.0;
  float4 gz00 = floor(gy00) / 6.0;
  gx00 = frac(gx00) - 0.5;
  gy00 = frac(gy00) - 0.5;
  gz00 = frac(gz00) - 0.5;
  float4 gw00 = float4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
  float4 sw00 = step(gw00, float4(0.0));
  gx00 -= sw00 * (step(0.0, gx00) - 0.5);
  gy00 -= sw00 * (step(0.0, gy00) - 0.5);

  float4 gx01 = ixy01 / 7.0;
  float4 gy01 = floor(gx01) / 7.0;
  float4 gz01 = floor(gy01) / 6.0;
  gx01 = frac(gx01) - 0.5;
  gy01 = frac(gy01) - 0.5;
  gz01 = frac(gz01) - 0.5;
  float4 gw01 = float4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
  float4 sw01 = step(gw01, float4(0.0));
  gx01 -= sw01 * (step(0.0, gx01) - 0.5);
  gy01 -= sw01 * (step(0.0, gy01) - 0.5);

  float4 gx10 = ixy10 / 7.0;
  float4 gy10 = floor(gx10) / 7.0;
  float4 gz10 = floor(gy10) / 6.0;
  gx10 = frac(gx10) - 0.5;
  gy10 = frac(gy10) - 0.5;
  gz10 = frac(gz10) - 0.5;
  float4 gw10 = float4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
  float4 sw10 = step(gw10, float4(0.0));
  gx10 -= sw10 * (step(0.0, gx10) - 0.5);
  gy10 -= sw10 * (step(0.0, gy10) - 0.5);

  float4 gx11 = ixy11 / 7.0;
  float4 gy11 = floor(gx11) / 7.0;
  float4 gz11 = floor(gy11) / 6.0;
  gx11 = frac(gx11) - 0.5;
  gy11 = frac(gy11) - 0.5;
  gz11 = frac(gz11) - 0.5;
  float4 gw11 = float4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
  float4 sw11 = step(gw11, float4(0.0));
  gx11 -= sw11 * (step(0.0, gx11) - 0.5);
  gy11 -= sw11 * (step(0.0, gy11) - 0.5);

  float4 g0000 = float4(gx00.x,gy00.x,gz00.x,gw00.x);
  float4 g1000 = float4(gx00.y,gy00.y,gz00.y,gw00.y);
  float4 g0100 = float4(gx00.z,gy00.z,gz00.z,gw00.z);
  float4 g1100 = float4(gx00.w,gy00.w,gz00.w,gw00.w);
  float4 g0010 = float4(gx10.x,gy10.x,gz10.x,gw10.x);
  float4 g1010 = float4(gx10.y,gy10.y,gz10.y,gw10.y);
  float4 g0110 = float4(gx10.z,gy10.z,gz10.z,gw10.z);
  float4 g1110 = float4(gx10.w,gy10.w,gz10.w,gw10.w);
  float4 g0001 = float4(gx01.x,gy01.x,gz01.x,gw01.x);
  float4 g1001 = float4(gx01.y,gy01.y,gz01.y,gw01.y);
  float4 g0101 = float4(gx01.z,gy01.z,gz01.z,gw01.z);
  float4 g1101 = float4(gx01.w,gy01.w,gz01.w,gw01.w);
  float4 g0011 = float4(gx11.x,gy11.x,gz11.x,gw11.x);
  float4 g1011 = float4(gx11.y,gy11.y,gz11.y,gw11.y);
  float4 g0111 = float4(gx11.z,gy11.z,gz11.z,gw11.z);
  float4 g1111 = float4(gx11.w,gy11.w,gz11.w,gw11.w);

  float4 norm00 = taylorInvSqrt(float4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
  g0000 *= norm00.x;
  g0100 *= norm00.y;
  g1000 *= norm00.z;
  g1100 *= norm00.w;

  float4 norm01 = taylorInvSqrt(float4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
  g0001 *= norm01.x;
  g0101 *= norm01.y;
  g1001 *= norm01.z;
  g1101 *= norm01.w;

  float4 norm10 = taylorInvSqrt(float4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
  g0010 *= norm10.x;
  g0110 *= norm10.y;
  g1010 *= norm10.z;
  g1110 *= norm10.w;

  float4 norm11 = taylorInvSqrt(float4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
  g0011 *= norm11.x;
  g0111 *= norm11.y;
  g1011 *= norm11.z;
  g1111 *= norm11.w;

  float n0000 = dot(g0000, Pf0);
  float n1000 = dot(g1000, float4(Pf1.x, Pf0.yzw));
  float n0100 = dot(g0100, float4(Pf0.x, Pf1.y, Pf0.zw));
  float n1100 = dot(g1100, float4(Pf1.xy, Pf0.zw));
  float n0010 = dot(g0010, float4(Pf0.xy, Pf1.z, Pf0.w));
  float n1010 = dot(g1010, float4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
  float n0110 = dot(g0110, float4(Pf0.x, Pf1.yz, Pf0.w));
  float n1110 = dot(g1110, float4(Pf1.xyz, Pf0.w));
  float n0001 = dot(g0001, float4(Pf0.xyz, Pf1.w));
  float n1001 = dot(g1001, float4(Pf1.x, Pf0.yz, Pf1.w));
  float n0101 = dot(g0101, float4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
  float n1101 = dot(g1101, float4(Pf1.xy, Pf0.z, Pf1.w));
  float n0011 = dot(g0011, float4(Pf0.xy, Pf1.zw));
  float n1011 = dot(g1011, float4(Pf1.x, Pf0.y, Pf1.zw));
  float n0111 = dot(g0111, float4(Pf0.x, Pf1.yzw));
  float n1111 = dot(g1111, Pf1);

  float4 fade_xyzw = fade(Pf0);
  float4 n_0w = lerp(float4(n0000, n1000, n0100, n1100), float4(n0001, n1001, n0101, n1101), fade_xyzw.w);
  float4 n_1w = lerp(float4(n0010, n1010, n0110, n1110), float4(n0011, n1011, n0111, n1111), fade_xyzw.w);
  float4 n_zw = lerp(n_0w, n_1w, fade_xyzw.z);
  float2 n_yzw = lerp(n_zw.xy, n_zw.zw, fade_xyzw.y);
  float n_xyzw = lerp(n_yzw.x, n_yzw.y, fade_xyzw.x);
  return 2.2 * n_xyzw;
}

// Classic Perlin noise, periodic version
float perlinc(float4 P, float4 rep){
  float4 Pi0 = modf(floor(P), rep); // Integer part modfulo rep
  float4 Pi1 = modf(Pi0 + 1.0, rep); // Integer part + 1 modf rep
  float4 Pf0 = frac(P); // Fractional part for interpolation
  float4 Pf1 = Pf0 - 1.0; // Fractional part - 1.0
  float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  float4 iy = float4(Pi0.yy, Pi1.yy);
  float4 iz0 = float4(Pi0.zzzz);
  float4 iz1 = float4(Pi1.zzzz);
  float4 iw0 = float4(Pi0.wwww);
  float4 iw1 = float4(Pi1.wwww);

  float4 ixy = permute(permute(ix) + iy);
  float4 ixy0 = permute(ixy + iz0);
  float4 ixy1 = permute(ixy + iz1);
  float4 ixy00 = permute(ixy0 + iw0);
  float4 ixy01 = permute(ixy0 + iw1);
  float4 ixy10 = permute(ixy1 + iw0);
  float4 ixy11 = permute(ixy1 + iw1);

  float4 gx00 = ixy00 / 7.0;
  float4 gy00 = floor(gx00) / 7.0;
  float4 gz00 = floor(gy00) / 6.0;
  gx00 = frac(gx00) - 0.5;
  gy00 = frac(gy00) - 0.5;
  gz00 = frac(gz00) - 0.5;
  float4 gw00 = float4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
  float4 sw00 = step(gw00, float4(0.0));
  gx00 -= sw00 * (step(0.0, gx00) - 0.5);
  gy00 -= sw00 * (step(0.0, gy00) - 0.5);

  float4 gx01 = ixy01 / 7.0;
  float4 gy01 = floor(gx01) / 7.0;
  float4 gz01 = floor(gy01) / 6.0;
  gx01 = frac(gx01) - 0.5;
  gy01 = frac(gy01) - 0.5;
  gz01 = frac(gz01) - 0.5;
  float4 gw01 = float4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
  float4 sw01 = step(gw01, float4(0.0));
  gx01 -= sw01 * (step(0.0, gx01) - 0.5);
  gy01 -= sw01 * (step(0.0, gy01) - 0.5);

  float4 gx10 = ixy10 / 7.0;
  float4 gy10 = floor(gx10) / 7.0;
  float4 gz10 = floor(gy10) / 6.0;
  gx10 = frac(gx10) - 0.5;
  gy10 = frac(gy10) - 0.5;
  gz10 = frac(gz10) - 0.5;
  float4 gw10 = float4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
  float4 sw10 = step(gw10, float4(0.0));
  gx10 -= sw10 * (step(0.0, gx10) - 0.5);
  gy10 -= sw10 * (step(0.0, gy10) - 0.5);

  float4 gx11 = ixy11 / 7.0;
  float4 gy11 = floor(gx11) / 7.0;
  float4 gz11 = floor(gy11) / 6.0;
  gx11 = frac(gx11) - 0.5;
  gy11 = frac(gy11) - 0.5;
  gz11 = frac(gz11) - 0.5;
  float4 gw11 = float4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
  float4 sw11 = step(gw11, float4(0.0));
  gx11 -= sw11 * (step(0.0, gx11) - 0.5);
  gy11 -= sw11 * (step(0.0, gy11) - 0.5);

  float4 g0000 = float4(gx00.x,gy00.x,gz00.x,gw00.x);
  float4 g1000 = float4(gx00.y,gy00.y,gz00.y,gw00.y);
  float4 g0100 = float4(gx00.z,gy00.z,gz00.z,gw00.z);
  float4 g1100 = float4(gx00.w,gy00.w,gz00.w,gw00.w);
  float4 g0010 = float4(gx10.x,gy10.x,gz10.x,gw10.x);
  float4 g1010 = float4(gx10.y,gy10.y,gz10.y,gw10.y);
  float4 g0110 = float4(gx10.z,gy10.z,gz10.z,gw10.z);
  float4 g1110 = float4(gx10.w,gy10.w,gz10.w,gw10.w);
  float4 g0001 = float4(gx01.x,gy01.x,gz01.x,gw01.x);
  float4 g1001 = float4(gx01.y,gy01.y,gz01.y,gw01.y);
  float4 g0101 = float4(gx01.z,gy01.z,gz01.z,gw01.z);
  float4 g1101 = float4(gx01.w,gy01.w,gz01.w,gw01.w);
  float4 g0011 = float4(gx11.x,gy11.x,gz11.x,gw11.x);
  float4 g1011 = float4(gx11.y,gy11.y,gz11.y,gw11.y);
  float4 g0111 = float4(gx11.z,gy11.z,gz11.z,gw11.z);
  float4 g1111 = float4(gx11.w,gy11.w,gz11.w,gw11.w);

  float4 norm00 = taylorInvSqrt(float4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
  g0000 *= norm00.x;
  g0100 *= norm00.y;
  g1000 *= norm00.z;
  g1100 *= norm00.w;

  float4 norm01 = taylorInvSqrt(float4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
  g0001 *= norm01.x;
  g0101 *= norm01.y;
  g1001 *= norm01.z;
  g1101 *= norm01.w;

  float4 norm10 = taylorInvSqrt(float4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
  g0010 *= norm10.x;
  g0110 *= norm10.y;
  g1010 *= norm10.z;
  g1110 *= norm10.w;

  float4 norm11 = taylorInvSqrt(float4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
  g0011 *= norm11.x;
  g0111 *= norm11.y;
  g1011 *= norm11.z;
  g1111 *= norm11.w;

  float n0000 = dot(g0000, Pf0);
  float n1000 = dot(g1000, float4(Pf1.x, Pf0.yzw));
  float n0100 = dot(g0100, float4(Pf0.x, Pf1.y, Pf0.zw));
  float n1100 = dot(g1100, float4(Pf1.xy, Pf0.zw));
  float n0010 = dot(g0010, float4(Pf0.xy, Pf1.z, Pf0.w));
  float n1010 = dot(g1010, float4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
  float n0110 = dot(g0110, float4(Pf0.x, Pf1.yz, Pf0.w));
  float n1110 = dot(g1110, float4(Pf1.xyz, Pf0.w));
  float n0001 = dot(g0001, float4(Pf0.xyz, Pf1.w));
  float n1001 = dot(g1001, float4(Pf1.x, Pf0.yz, Pf1.w));
  float n0101 = dot(g0101, float4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
  float n1101 = dot(g1101, float4(Pf1.xy, Pf0.z, Pf1.w));
  float n0011 = dot(g0011, float4(Pf0.xy, Pf1.zw));
  float n1011 = dot(g1011, float4(Pf1.x, Pf0.y, Pf1.zw));
  float n0111 = dot(g0111, float4(Pf0.x, Pf1.yzw));
  float n1111 = dot(g1111, Pf1);

  float4 fade_xyzw = fade(Pf0);
  float4 n_0w = lerp(float4(n0000, n1000, n0100, n1100), float4(n0001, n1001, n0101, n1101), fade_xyzw.w);
  float4 n_1w = lerp(float4(n0010, n1010, n0110, n1110), float4(n0011, n1011, n0111, n1111), fade_xyzw.w);
  float4 n_zw = lerp(n_0w, n_1w, fade_xyzw.z);
  float2 n_yzw = lerp(n_zw.xy, n_zw.zw, fade_xyzw.y);
  float n_xyzw = lerp(n_yzw.x, n_yzw.y, fade_xyzw.x);
  return 2.2 * n_xyzw;
}

// Generic 1,2,3 Noise

float rand(float n){return frac(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
    float fc = frac(p);
	return lerp(rand(fl), rand(fl + 1.0), fc);
}

float noise(float2 p){
	float2 ip = floor(p);
	float2 u = frac(p);
	u = u*u*(3.0-2.0*u);

	float res = lerp(
		lerp(rand(ip),rand(ip+float2(1.0,0.0)),u.x),
		lerp(rand(ip+float2(0.0,1.0)),rand(ip+float2(1.0,1.0)),u.x),u.y);
	return res*res;
}

float modf289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
float4 modf289(float4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
float4 perm(float4 x){return modf289(((x * 34.0) + 1.0) * x);}

float noise(float3 p){
    float3 a = floor(p);
    float3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    float4 b = a.xxyy + float4(0.0, 1.0, 0.0, 1.0);
    float4 k1 = perm(b.xyxy);
    float4 k2 = perm(k1.xyxy + b.zzww);

    float4 c = k2 + a.zzzz;
    float4 k3 = perm(c);
    float4 k4 = perm(c + 1.0);

    float4 o1 = frac(k3 * (1.0 / 41.0));
    float4 o2 = frac(k4 * (1.0 / 41.0));

    float4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    float2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}
)###";

#endif //XENGINE_HLSL_NOISE_HPP
