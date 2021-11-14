#include <iostream>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"
#include "../lib/stb/stb_image_resize.h"
#include "../lib/stb/stb_image_write.h"


enum class Direction
{
	None,
	Width,
	Height,
};

class Option
{
public:
	Direction dir;
	int pixels;

public:
	Option(void) :
		dir(Direction::None),
		pixels(0)
	{
	}

	bool invalid(void) const
	{
		return pixels <= 0;
	}

	static Option getInvalid(void)
	{
		return Option();
	}

	string getCmd(void) const
	{
		return getDirStr() + to_string(pixels);
	}

	string getDirStr(void) const
	{
		switch (dir)
		{
		case Direction::Width: return "w";
		case Direction::Height: return "h";
		default:
			return "";
		}
	}
};

Option parseOption(const string& s)
{
	if (s.length() < 2)
		return Option::getInvalid();

	Option o;
	char h = s[0];
	if (h == 'w' || h == 'W')
		o.dir = Direction::Width;
	else if (h == 'h' || h == 'H')
		o.dir = Direction::Height;
	else
		return Option::getInvalid();

	o.pixels = atoi(&s[1]);

	return o;
}

string getFnOut(const string& fn, const Option& opt)
{
	string::size_type i = fn.find_last_of('.');
	return fn.substr(0, i) + + "_" + opt.getCmd() + ".png";
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "invalid arguments" << endl;
		return 1;
	}

	Option opt = parseOption(argv[1]);
	if (opt.invalid())
	{
		cout << "invalid option" << endl;
		return 2;
	}

	for (int i = 2; i < argc; ++i)
	{
		string fn = argv[i];
		string fnOut = getFnOut(fn, opt);

		int w, h, comp;
		stbi_uc* buf = stbi_load(fn.c_str(), &w, &h, &comp, 0);
		if (!buf)
		{
			cout << "failed to load : " << fn << endl;
			continue;
		}

		int ow, oh;
		if (opt.dir == Direction::Width)
		{
			ow = opt.pixels;
			oh = h * opt.pixels / w;
		}
		else if (opt.dir == Direction::Height)
		{
			oh = opt.pixels; 
			ow = w * opt.pixels / h;
		}
		else
		{
			cout << "invalid request" << endl;
			continue;
		}

		vector<stbi_uc> bufOut(ow * oh * comp);

		stbir_resize(
			buf,
			w,
			h,
			0,
			&bufOut[0],
			ow,
			oh,
			0,
			STBIR_TYPE_UINT8,
			comp,
			-1,
			0,
			STBIR_EDGE_WRAP,
			STBIR_EDGE_WRAP,
			STBIR_FILTER_DEFAULT,
			STBIR_FILTER_DEFAULT,
			STBIR_COLORSPACE_LINEAR,
			0);

		stbi_write_png(
			fnOut.c_str(), ow, oh, comp, &bufOut[0], 0);
	}

	return 0;
}
