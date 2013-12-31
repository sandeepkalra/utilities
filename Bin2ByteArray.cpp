#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 3) 
	{
	cout<<"Usage: Bin2ByteArray $bin_file $array_file"<<endl;
	return 0;
	}
	ifstream in(argv[1], ios::in | ifstream::binary);
	if(!in) {
	cout<<"Failed to open "<<argv[1] <<" file. "<<endl;
	return 0 ;
	}
	ofstream out(argv[2], ios::out );
	in.seekg(0,in.end);
	int length = in.tellg();
	in.seekg(0,in.beg);
	char *buffer  = new char[length];
	in.read(buffer, length);
	if(in.is_open() && out.is_open())
	{
		int l=0;
		int tab = 0;
		char * p = buffer;
		out << "/*Copyright(c) 2014 - Sandeep Kalra"<<endl;
		out << "This file is prepared using www.github.com/sandeepkalra/utilities "<<endl<<"*/"<<endl;
		out << "const char *bin_array[] =  { "<<endl;
		while(l < length)
		{
			int val = (*p) & 0x000000ff;
			out<<hex<<"0x"<<val;
			++l;++p;++tab;
			if(l<length) out<<",\t";
			if(10==tab)  { out<<endl; tab=0;}
		}
		out<<endl<<"};"<<endl;
		in.close(); out.close();
	}
	delete[] buffer;
return 0;
}
