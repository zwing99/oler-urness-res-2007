#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

using namespace std;

ifstream inputData;
ofstream outputData;

int main(int argc, char** argv)
{

	int datatoread[14];
	int i;
	for(i = 0; i < 14 ; i++)
	{
		datatoread[i] = 0;
	}
	char* my_datafile;
	if(argc>1)
		inputData.open(argv[1],ios::in);
	else
	{
		cout<<"Must Specify Datafile"<<endl;
		return 0;
	}
	if(argc>2)
		outputData.open(argv[2],ios::out);
	else
		outputData.open("formateddata.txt",ios::out);
	if(argc>3)
	{
		int turnon;
		for(i = 3; i<argc; i++)
		{
			sscanf(argv[i],"%d",&turnon);
			if(turnon>13 || turnon <0)
			{
				cout<<"Bad Columns"<<endl;
				return 0;
			}
			datatoread[turnon] = 1;
		}
		
	}
	
	for(i = 0; i < 14 ; i++)
	{
		cout<<datatoread[i]<<" ";
	}
	cout<<endl;
	
	char head[200];
	float readin[14];
	int iter;
	//for(i = 0; i<14; i++);
	//	readin[i] = new char[20];
	cout<<readin<<endl;
	
	if(inputData.good())
	{
		char temp;
		char temp2;
		inputData.getline((char*)&head,200);
		if(head[0] == '#')
		{
			//char read0[20],read1[20],read2[20],read3[20],read4[20],read5[20],read6[20],read7[20],read8[20]
			//,read9[20],read10[20],read11[20],read12[20],read13[20];
			char* readin[14];
			for(i = 0; i<14; i++)
				readin[i] = new char[20];
			i = sscanf((char*)&head, "%c %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",&temp,
				   readin[0],readin[1],readin[2],readin[3],readin[4],readin[5],readin[6],
				   readin[7],readin[8],readin[9],readin[10],readin[11],readin[12],readin[13]);
			outputData<<"# ";
			for(i = 0; i < 14; i++)
				if(datatoread[i])
					outputData<<readin[i]<<" ";
			outputData<<"\b"<<endl;
		}
		else
		{
			i = sscanf((char*)&head, "%d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
					   &iter,&readin[1],&readin[2],&readin[3],&readin[4],&readin[5],&readin[6],
					   &readin[7],&readin[8],&readin[9],&readin[10],&readin[11],&readin[12],&readin[13]);
			int j=1;
			if(datatoread[0])
				outputData<<iter;
			else
			{
				j=1;
				while(!datatoread[j])
				{
					j++;
					if(j>13)
						break;
				}
				outputData<<readin[j];
			}
			for(i=j+1; i < 14; i++)
				if(datatoread[i])
					outputData<<" "<<readin[i];
			outputData<<"\n";
		}
	}
	while(inputData.good())
	{
		inputData.getline((char*)&head,200);
		i = sscanf((char*)&head, "%d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				   &iter,&readin[1],&readin[2],&readin[3],&readin[4],&readin[5],&readin[6],
				   &readin[7],&readin[8],&readin[9],&readin[10],&readin[11],&readin[12],&readin[13]);
		int j=1;
		if(datatoread[0])
			outputData<<iter;
		else
		{
			j=1;
			while(!datatoread[j])
			{
				j++;
				if(j>13)
					break;
			}
			outputData<<readin[j];
		}
		for(i=j+1; i < 14; i++)
			if(datatoread[i])
				outputData<<" "<<readin[i];
		outputData<<"\n";
	}
	
	/*
	int count;
	float TE,ME,DE,TMP,RTMP,DTH,RDTH,MX,MY,MZ,MTX,MTY,MTZ;
	if(inputData.good())
	while(inputData.good())
	{
		inputData.getline((char*)&head,200);
		//int q = sprintf((char*)&head,"%d %f %f %f %f %f %f %f %f %f %f %f %f %f",
		//							count,TE,ME,DE,TMP,RTMP,DTH,RDTH,MX,MY,MZ,MTX,MTY,MTZ);
		cout<<head<<endl;
		//cout<<MTZ<<endl;
		//system(my_func);
	}
	*/
	return 0;
}

				 // <<counter<<" "<<totalEnergy<<" "<<minEnergy<<" "<<totalEnergy-minEnergy<<" "
				 // <<temperature<<" "<<realtemperature<<" "
				 // <<dtheta<<" "<<realdtheta<<" "
				 // <<magX<<" "<<magY<<" "<<magZ<<" "
				 // <<u_component<<" "<<v_component<<" "<<w_component<<endl;