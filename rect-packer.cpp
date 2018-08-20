#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>

int imax(int a, int b)
{
	return a > b ? a : b;
}

struct Rectangle
{
	// input
	int width;
	int height;
	
	// output
	int x;
	int y;
	
	Rectangle(int width, int height)
		: width(width)
		, height(height)
		, x(0)
		, y(0)
		{}
};

// pack all rectangles in-place
void packRectangles(std::vector<Rectangle> &rectangles, int &outputWidth, int &outputHeight)
{
	outputWidth = 0;
	outputHeight = 0;
	
	int totalArea = 0;
	
	// estimate minimum bounds and total area
	for (const auto &rectangle: rectangles)
	{
		outputWidth = imax(outputWidth, rectangle.width);
		outputHeight = imax(outputHeight, rectangle.height);
		totalArea += rectangle.width * rectangle.height;
	}
	
	// estimate border length from total area (assume full packing efficiency)
	int borderLength = (int)sqrt((double)totalArea);
	
	// round up to next power of two
	int nextBorderLength = 0;
	for (int i = 1; i < borderLength; i *= 2)
		nextBorderLength = i;
	
	// width cannot be smaller than the largest rectangle
	outputWidth = imax(outputWidth, nextBorderLength);
	
	// sort rectangles by relative perimeter
	std::sort(rectangles.begin(), rectangles.end(), [](const Rectangle &lhs, const Rectangle &rhs)
	{
		return lhs.width + lhs.height < rhs.width + rhs.height;
	});
	
	// stupid starting algorithm
	int y = 0;
	for (auto &rectangle: rectangles)
	{
		rectangle.y = y;
		y += rectangle.height;
		outputHeight = imax(outputHeight, y);
	}
}

unsigned char bmpHeader[] = {
	0x42, 0x4d, 0xd6, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00,
	0x00, 0x00, /* width (LE, short) */
	0x00, 0x00,
	0x00, 0x00, /* height (LE, short) */
	0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void saveDebugImage(const std::string &filename, int width, int height, const std::vector<Rectangle> &rectangles)
{
	unsigned char *data = new unsigned char[width * height * 3];
	
	memset(data, 0, width * height * 3);
	
	// rasterize each rectangle into bitmap form
	for (const auto &rectangle: rectangles)
	{
		unsigned char color = rand() % 128 + 128;
		for (int y = rectangle.y; y < rectangle.y + rectangle.height; y++)
		{
			for (int x = rectangle.x; x < rectangle.x + rectangle.width; x++)
			{
				int offset = (y * width + x) * 3;
				
				// image bounds check
				assert(x >= 0);
				assert(y >= 0);
				assert(x < width);
				assert(y < height);
				
				// overlap check
				assert(data[offset + 0] == 0);
				assert(data[offset + 1] == 0);
				assert(data[offset + 2] == 0);
				
				// write pixel
				data[offset++] = color;
				data[offset++] = color;
				data[offset++] = color;
			}
		}
	}
	
	// write basic BMP file
	FILE *f = fopen(filename.c_str(), "wb");
	assert(f != nullptr);
	
	*(unsigned short *)(bmpHeader + 18) = (unsigned short)width;
	*(unsigned short *)(bmpHeader + 22) = (unsigned short)height;
	fwrite(bmpHeader, sizeof(bmpHeader), 1, f);
	
	fwrite(data, width * height * 3, 1, f);
	
	fclose(f);
	
	delete [] data;
}

int main()
{
	std::vector<Rectangle> rectangles;
	
	for (int i = 0; i < 200; i++)
	{
		int width = rand() % 256;
		int height = rand() % 256;
		rectangles.push_back(Rectangle(width, height));
	}
	
	int imageWidth, imageHeight;
	packRectangles(rectangles, imageWidth, imageHeight);
	
	saveDebugImage("plop.bmp", imageWidth, imageHeight, rectangles);
	
	return 0;
}
