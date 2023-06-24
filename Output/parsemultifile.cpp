#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

using namespace std;

//ofstream outputData;

int main(int argc, char** argv)
{
	char* my_datafile;
	my_datafile = "mydata";
	char my_func[100];
	float magfield = .01;
	float temperature = .01;
	for(magfield = .05; magfield <= .1; magfield += .01)
	{
		for(temperature = 0; temperature <= 0; temperature += 1)
		{
			int magfieldnum = (int)(magfield*100);
			int tempnum = (int)(temperature*100);
			//cout<<magfieldnum<<"_"<<tempnum<<endl;
			int q = sprintf((char*)&my_func,"./ParseFile %s%d_%d.txt formateddata%d_%d.txt 0 11",my_datafile,magfieldnum,tempnum,magfieldnum,tempnum);
			cout<<my_func<<endl;
			system(my_func);
		}
	}
	return 0;
}