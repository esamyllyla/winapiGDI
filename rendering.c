
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

static int init = 0;

typedef struct
{
	float x;
    float y;
} vertex2;

typedef struct
{
	vertex2 v1;
    vertex2 v2;
} vec2pairs;

typedef struct
{
	float x;
    float y;
    float z;
} vertex3;

typedef struct
{
    int width;
    int height;
    uint32_t *pixels;
} Frame;

vertex3 cubes[8] = 
{
	{-0.5, -0.5, 1.0},
	{-0.5,  0.5, 1.0},
    { 0.5, -0.5, 1.0},
    { 0.5,  0.5, 1.0},
	{-0.5, -0.5, 2.0},
	{-0.5,  0.5, 2.0},
    { 0.5, -0.5, 2.0},
    { 0.5,  0.5, 2.0},
};

void rotate(float *x, float *y, float cx, float cy, float dt)
{
	float dx = *x - cx;
    float dy = *y - cy;
    
    float r = sqrt(dx*dx + dy*dy);
    float angle = atan2(dy, dx) + dt;
    
    *x = cx + r*cos(angle);
    *y = cy + r*sin(angle);
}

void rectangle(Frame *frame, float width, float height, float centerx, float centery, uint32_t color)
{
	float top = centery - (height / 2.0f);
    float bot = centery + (height / 2.0f);
    float left = centerx - (width / 2.0f);
    float right = centerx + (width / 2.0f);
    
    if(top < 0) top = 0;
    if(bot < 0) bot = 0;
    if(left < 0) left = 0;
    if(right < 0) right = 0;
    if(top > frame->height) top = frame->height;
    if(bot > frame->height) bot = frame->height;
    if(left > frame->width) left = frame->width;
    if(right > frame->width) right = frame->width;
    
    for(int x = left; x < right; x++)
    {
    	for(int y = top; y < bot; y++)
        {
        	frame->pixels[(y * frame->width) + x] = color;
        }
    }
}

void shallowLine(Frame *frame, float x0, float x1, float y0, float y1, float dy, float dx)
{
	float y = y0;
    for(int x = (int)x0; x < (int)x1; x++)
    {
        y += (dy/dx);
        frame->pixels[((int)y * frame->width) + x] = 0xFFFFFFFF;
    }
}

void steepLine(Frame *frame, float x0, float x1, float y0, float y1, float dy, float dx)
{
    float x = x0;
    for(int y = (int)y0; y < (int)y1; y++)
    {
        x += (dx/dy);
        frame->pixels[(y * frame->width) + (int)x] = 0xFFFFFFFF;
    }
}

void drawLine(Frame *frame, vertex2 node0, vertex2 node1)
{
	float x0, x1, y0, y1;
    
    if(node0.x < node1.x)
    {
    	x0 = node0.x;
        y0 = node0.y;
        x1 = node1.x;
        y1 = node1.y;
        
        float dx = x1 - x0;
        float dy = y1 - y0;
        
        if(node0.y < node1.y)
        {
        	if(dx >= dy)
            {
                shallowLine(frame, x0, x1, y0, y1, dy, dx);        	
            }
            if(dx < dy)
            {
                steepLine(frame, x0, x1, y0, y1, dy, dx);
            }
        }
        if(node0.y >= node1.y)
        {
        	if(dx >= -dy)
            {
                shallowLine(frame, x0, x1, y0, y1, dy, dx);        	
            }
            if(dx < -dy)
            {
                steepLine(frame, x1, x0, y1, y0, dy, dx);
            }
        }
    }
    
    if(node0.x > node1.x)
    {
    	x0 = node1.x;
        y0 = node1.y;
        x1 = node0.x;
        y1 = node0.y;
        
        float dx = x1 - x0;
        float dy = y1 - y0;
        
        if(y0 < y1)
        {
        	if(dx >= dy)
            {
                shallowLine(frame, x0, x1, y0, y1, dy, dx);        	
            }
            if(dx < dy)
            {
                steepLine(frame, x0, x1, y0, y1, dy, dx);
            }
        }
        if(y0 >= y1)
        {
        	if(dx >= -dy)
            {
                shallowLine(frame, x0, x1, y0, y1, dy, dx);        	
            }
            if(dx < -dy)
            {
                steepLine(frame, x1, x0, y1, y0, dy, dx);
            }
        }
    }
}

void render(Frame *frame, float dt)
{
	rectangle(frame, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 0xFF200040);

    vertex2 projCubeCorners[8] = {0, 0};
    
    for(int i = 0; i < 8; i++)
    {
    	float HalfWidth = (float)WIDTH / 2.0f;
        float HalfHeight = (float)HEIGHT / 2.0f;

		rotate(&cubes[i].x, &cubes[i].y, 0.0f, 0.0f, 0.001f);
        rotate(&cubes[i].x, &cubes[i].z, 0.0f, 1.5f, 0.001f);
                
        float x = ((cubes[i].x / cubes[i].z) * HalfWidth) + HalfWidth;
        float y = ((cubes[i].y / cubes[i].z) * HalfHeight) + HalfHeight;
        
        projCubeCorners[i].x = x;
        projCubeCorners[i].y = y;
    }
    
    vec2pairs Edges[12] =
    {
    	{projCubeCorners[0], projCubeCorners[1]},
        {projCubeCorners[0], projCubeCorners[2]},
        {projCubeCorners[0], projCubeCorners[4]},
        {projCubeCorners[1], projCubeCorners[3]},
        {projCubeCorners[1], projCubeCorners[5]},
        {projCubeCorners[5], projCubeCorners[4]},
        {projCubeCorners[5], projCubeCorners[7]},
        {projCubeCorners[7], projCubeCorners[3]},
        {projCubeCorners[7], projCubeCorners[6]},
        {projCubeCorners[6], projCubeCorners[2]},
        {projCubeCorners[2], projCubeCorners[3]},
        {projCubeCorners[6], projCubeCorners[4]},
    };
    
    for(int i = 0; i < 12; i++)
    {
    	drawLine(frame, Edges[i].v1, Edges[i].v2);
    }
}
