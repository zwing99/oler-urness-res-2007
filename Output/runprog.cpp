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
	//outputData.open(datafile,ios::out);
	//outputData.close();
	char my_func[100];
	float magfield = .01;
	float temperature = .01;
	for(magfield = .1; magfield <= 0.5; magfield += .1)
	{
		for(temperature = 0.22; temperature <= 0.301; temperature += .02)
		{
			int magfieldnum = (int)(magfield*100);
			int tempnum = (int)(temperature*100);
			//cout<<magfieldnum<<"_"<<tempnum<<endl;
			//int q = sprintf((char*)&my_func,"./Output %s%d_%d.txt %f %f",my_datafile,magfieldnum,tempnum,magfield,temperature);
			int q = sprintf((char*)&my_func,"./Output %s.txt %f %f",my_datafile,magfield,temperature);
			cout<<my_func<<endl;
			system(my_func);
		}
	}
	return 0;
}