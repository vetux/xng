#version 460

layout(binding = 0, std140) uniform ShaderData
{
    dvec4 cx_cy_scale;
    ivec4 maxIterationsColorMode;
    vec4 color;
    vec4 offset;
} vars;

layout(binding = 1) uniform writeonly image2D outImage;

vec4 getColorPalette0(int iterations, int maxIterations) {
    if (iterations < maxIterations && iterations > 0) {
        vec4 colors[16];
        colors[0] = vec4(66.0f / 255, 30.0f / 255, 15.0f / 255, 1);
        colors[1] = vec4(25.0f / 255, 7.0f / 255, 26.0f / 255, 1);
        colors[2] = vec4(9.0f / 255, 1.0f / 255, 47.0f / 255, 1);
        colors[3] = vec4(4.0f / 255, 4.0f / 255, 73.0f / 255, 1);
        colors[4] = vec4(0.0f / 255, 7.0f / 255, 100.0f  / 255, 1);
        colors[5] = vec4(12.0f / 255, 44.0f / 255, 138.0f / 255, 1);
        colors[6] = vec4(24.0f / 255, 82.0f / 255, 177.0f / 255, 1);
        colors[7] = vec4(57.0f / 255, 125.0f / 255, 209.0f / 255, 1);
        colors[8] = vec4(134.0f / 255, 181.0f / 255, 229.0f  / 255, 1);
        colors[9] = vec4(211.0f / 255, 236.0f / 255, 248.0f / 255, 1);
        colors[10] = vec4(241.0f / 255, 233.0f / 255, 191.0f / 255, 1);
        colors[11] = vec4(248.0f / 255, 201.0f / 255, 95.0f / 255, 1);
        colors[12] = vec4(255.0f / 255, 170.0f / 255, 0.0f / 255, 1);
        colors[13] = vec4(204.0f / 255, 128.0f/ 255, 0.0f / 255, 1);
        colors[14] = vec4(153.0f / 255, 87.0f / 255, 0.0f  / 255, 1);
        colors[15] = vec4(106.0f / 255, 52.0f / 255, 3.0f / 255, 1);
        int indexF = iterations % 16;
        return colors[indexF];
    } else {
        return vec4(0, 0, 0, 1);
    }
}

vec4 getColorSmooth(double x, double y, int iterations, int maxIterations, vec4 color) {
    if (iterations < maxIterations) {
        double z = sqrt(x * x + y * y);
        double nsmooth = iterations + 1 - double(log(log(float(z)))) / log(2);
        double v = nsmooth / maxIterations;
        return vec4(color.r * v, color.g * v, color.b * v, 1);
    } else {
        return vec4(0, 0, 0, 1);
    }
}

vec4 getPixel(double x, double y, int iterations, int maxIterations, int colorMode, vec4 color){
    if (colorMode == 0) {
        return getColorPalette0(iterations, maxIterations);
    } else {
        return getColorSmooth(x, y, iterations, maxIterations, color);
    }
}

vec4 mandelbrot(double cx, double cy, double Px, double Py, ivec2 size, double scale, int maxIterations, int colorMode, vec4 color) {
    int width = size.x;
    int height = size.y;

    double x0 = cx + Px * scale;
    double y0 = cy + Py * scale;
    double x = 0;
    double y = 0;
    int iterations = 0;
    while (x * x + y * y <= 4.0f && iterations < maxIterations) {
        double xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iterations++;
    }
    return getPixel(x, y, iterations, maxIterations, colorMode, color);
}

void main() {
    ivec2 imageSize = imageSize(outImage);

    double Px = double(gl_WorkGroupID.x) - (imageSize.x / 2);
    double Py = double(gl_WorkGroupID.y) - (imageSize.y / 2);
    double cx = vars.cx_cy_scale.x;
    double cy = vars.cx_cy_scale.y;
    double scale = vars.cx_cy_scale.z;
    int maxIterations = vars.maxIterationsColorMode.x;
    int colorMode = vars.maxIterationsColorMode.y;

    vec4 color = mandelbrot(cx, cy, Px, Py, imageSize, scale, maxIterations, colorMode, vars.color);

    imageStore(outImage, ivec2(gl_WorkGroupID.x, gl_WorkGroupID.y), color);
}
