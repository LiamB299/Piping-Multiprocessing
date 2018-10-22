// Liam Burgess U18015001


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int WRITE_END=1;
const int READ_END=0;
const int LENGTH=78;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main() {
	int forw[2];
	pid_t proc;	//process ID
	char cfile[25], send[100]; 
	string spass, sfile;	
	ifstream infile; //textfile
	string sline, snum, sup;
	int icount=0;
	stringstream ss;

	string arr[100][3];

	//cout<<"File name: ";
	//cin >>sline
	//infile.open(sline.c_str())

	strcpy(cfile, "device_data.txt");
	infile.open(cfile);
	
	if (pipe(forw)==-1) {
		cout<<"Creation of pipes failed";
		exit(0);
	}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	while (infile>>sline) {
		strcpy(send, sline.c_str());
		icount++;
		ss<<icount;
		snum = ss.str();

		cout<<"before fork: "<<send<<endl;

		pipe(forw);
		proc = fork();
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		switch (proc) {
			case -1 : { 
						cout<<"fork error";
						exit(0);
					}

			break;
			case 0 : {
						string sline, spass;
						int sec; 
						char receive[100], cs[100];
						char place[100], state[100];
						read(forw[READ_END], receive, LENGTH);				//read in data
						close(forw[READ_END]);
						cout<<"Child received message line: "<<receive<<endl;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

						sline.assign(receive);
						spass = sline.substr(0, sline.find(','));
						strcpy(place, spass.c_str());					//extract device
						place[spass.length()+1]= '\0';
						sline.erase(0, spass.length()+1);
						spass="";

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

						spass = sline.substr(0, sline.find(','));
						strcpy(state, spass.c_str());				//extract status
						sline.erase(0, spass.length()+1);					
						spass="";
						
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

						stringstream ss(sline);
						ss >> sec;
						strcpy(send, sline.c_str());

						time_t ep = (time_t)sec;
						char epoch[150];								//Extract time
						struct tm tstruct = *localtime(&ep);
						strftime(epoch, sizeof(epoch), "%H:%M:%S %d/%m/%Y", &tstruct);

						write(forw[WRITE_END], place, LENGTH);
						cout<<"Child sent device: "<<place<<endl;
						write(forw[WRITE_END], state, LENGTH);
						cout<<"Child sent value: "<<state<<endl;
						write(forw[WRITE_END], epoch, LENGTH);
						cout<<"Child sent time: "<<epoch<<endl;

						close(forw[WRITE_END]);
						close(forw[READ_END]);

						exit(0);
					}	
			break;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			default : {		//parent
					char time[100], place[100], state[100];

					strcpy(send, sline.c_str());
					cout<<"Parent sent file line: "<<icount<<endl;

					write(forw[WRITE_END], send, 1+ strlen(send));
					close(forw[WRITE_END]);											//pass data line
					wait(NULL);

					read(forw[READ_END], place, LENGTH);
					read(forw[READ_END], state, LENGTH);
					read(forw[READ_END], time, LENGTH);	

					string spla; 
					spla.assign(place);

					if (spla=="FrontDoor") {
						arr[0][0]=spla;
						arr[0][1].assign(state);
						cout<<"Something: "+arr[0][1];
						arr[0][2].assign(time);
					}	
					else if (spla=="LivingLight") {
						arr[1][0]=spla;
						arr[1][1].assign(state);
						arr[1][2].assign(time);
					}	
					else if (spla=="LivingTemp") {
						arr[2][0]=spla;
						arr[2][1].assign(state);
						arr[2][2].assign(time);
					}

					cout<<"Parent received update for "<<place<<": "<<state<<" at "<<time<<endl; 
					}	//inner switch
			} //switch

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		}//while
	cout<<"=================="<<endl;
	cout<<"Device List:"<<endl;
	cout<<"=================="<<endl;

	for (int iloop=0; iloop<3; iloop++) {
		cout<<arr[iloop][0]+":" +'\t' +arr[iloop][1]+"  @ "+arr[iloop][2]<<endl;
	} //for
	infile.close();
	return 0;
} //end
