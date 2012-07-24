
//Simple png read example using pngpp
//Developed by TFT

#include <iostream>
#include "png++/png.hpp"

using namespace std;
using namespace png;

void spitOut(bool bit)
{
	static unsigned short data=0;
	static int ca=0,cb=0;
	if(bit) data |= 0x8000;
	if(++ca==16)
	{
		ca=0;
		cout<<"0x"<<hex<<data<<",";
		if(++cb==8)
		{
			cb=0;
			cout<<endl;
		}
		data=0;
	} else data>>=1;
}

int main(int argc, char *argv[])
{
	//Constructor with one string parameter: open a png file
	image<rgb_pixel> img(argv[1]);

	cout<<endl<<"static const unsigned short poulimagedata[]={"<<endl;
	const int w=img.get_width();
	const int h=img.get_height();
	cerr<<w<<' '<<h<<endl;
	for(int j=0;j<h/2;j++)
	{
		for(int i=0;i<2*w;i++)
		{
			rgb_pixel pixel=img.get_pixel(i % w,j + h/2*(i/w));
			spitOut(pixel.red<128);
		}
	}
	cout<<"};"<<endl;
}
