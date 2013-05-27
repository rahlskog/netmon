#include <iostream>
#include <fstream>
#include <stdint.h>
#include <time.h>
#include "interfaces.h"

using std::cout;
using std::endl;

const int run_interval = 60;

void Usage(std::string name);
int MainLoop(Interface& interface);

int main(int argc, char **argv) {
	if (argc < 2) {
		Usage(argv[0]);
		return -1;
	}
	
	try {
		SysfsInterface interface(argv[1]);
		return MainLoop(interface);
	}
	catch (InterfaceException e) {
		cout << "Error: " << e.what() << endl;
		Usage(argv[0]);
		return -1;
	}
}

void Usage(std::string name) {
	cout << "Usage:\n  " << name << " [interface]\n" << endl;
}

int MainLoop(Interface& interface) {
	std::ifstream ifile(interface.IfName()+".log");
	if (!ifile)
	{
		std::ofstream ofile(interface.IfName()+".log");
		ofile << "#timestamp,time-delta,rx-delta,tx-delta" << endl;
	}
	ifile.close();
	
	timespec now{0,0};
	timespec wait{0,0};
	timespec remain{0,0};
	
	if (!interface.Update()) {
		cout << "Failed reading initial values" << endl;
		return -1;
	}
	
	clock_gettime(CLOCK_REALTIME, &now);
	wait.tv_sec = run_interval - (now.tv_sec%run_interval);
	
	for(;;)
	{
		if (nanosleep(&wait, &remain) == -1)
		{
			cout << "Sleep interrupted, napping some more" << endl;
			nanosleep(&remain, 0);
		}
		clock_gettime(CLOCK_REALTIME, &now);

		if (!interface.Update()) {
			cout << "Failed reading current valuies" << endl;
			return -1;
		}
		
		std::ofstream log(interface.IfName()+".log", std::ios::app);
		
		log << now.tv_sec << "," << wait.tv_sec << ",";
		log << interface.RxDelta() << "," << interface.TxDelta() << endl;
		
		log.close();
		
		wait.tv_sec = run_interval - (now.tv_sec%run_interval);
	}
	return 0;
}
