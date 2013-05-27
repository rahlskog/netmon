#include <iostream>
#include <fstream>
#include <stdint.h>
#include <time.h>
#include "interfaces.h"
#include "builtins.h"
using std::cout;
using std::endl;

const int run_interval = 60;

void Usage(std::string name);
int MainLoop(StatReader& iface, LogWriter& log);

int main(int argc, char **argv) {
	if (argc < 2) {
		Usage(argv[0]);
		return -1;
	}
	
	try {
		FileWriter log;
		SysfsReader interface(argv[1]);
		return MainLoop(interface, log);
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

int MainLoop(StatReader& iface, LogWriter& log) {
	LogPackage lp;
	lp.IfName = iface.IfName();
	timespec now{0,0};
	timespec wait{0,0};
	timespec remain{0,0};
	
	if (!iface.Update()) {
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

		if (!iface.Update()) {
			cout << "Failed reading current valuies" << endl;
			return -1;
		}
		
		lp.Time = now.tv_sec;
		lp.TimeDelta = wait.tv_sec;
		lp.RxDelta = iface.RxDelta();
		lp.TxDelta = iface.TxDelta();
		log.Log(lp);
		
		wait.tv_sec = run_interval - (now.tv_sec%run_interval);
	}
	return 0;
}
