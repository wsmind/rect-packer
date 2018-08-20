#include <vector>
#include <cstdlib>
#include <cassert>

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
void packRectangles(std::vector<Rectangle> &rectangles)
{
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
				/*assert(data[offset + 0] == 0);
				assert(data[offset + 1] == 0);
				assert(data[offset + 2] == 0);*/
				
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
	
	packRectangles(rectangles);
	
	saveDebugImage("plop.bmp", 256, 256, rectangles);
	
	return 0;
}
