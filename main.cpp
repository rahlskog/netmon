/*
 * Copyright (C) 2013 Robert Ahlskog.
 * 
 * This file is part of netmon, distributed under GNU GPL v3.
 * For full terms see included LICENSE file.
 */
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <time.h>
#include "interfaces.h"
#include "builtins.h"
using std::cout;
using std::endl;

void Usage(std::string name);
int MainLoop(StatReader& iface, int interval, LogWriter& log);

int main(int argc, char **argv) {
	int run_interval = 60;
	if (argc < 2) {
		Usage(argv[0]);
		return -1;
	}
	
	try {
		FileWriter log;
		SysfsReader interface(argv[1]);
		if (argc == 3)
		{
			int n = std::stoi(argv[2]);
			if (n < 5 || n > 3600)
				cout << "Error: Interval of " << n << " seconds is outside of range (5-3600)" << endl;
			run_interval = n;
		}
		return MainLoop(interface, run_interval, log);
	}
	catch (std::logic_error e)
	{
		cout << "Error: " << e.what() << endl;
		Usage(argv[0]);
		return -1;
	}
	catch (std::runtime_error e) {
		cout << "Error: " << e.what() << endl;
		Usage(argv[0]);
		return -1;
	}
	
}

void Usage(std::string name) {
	cout << "Usage:\n  " << name << " <interface> [interval]\n" << endl;
}

int MainLoop(StatReader& iface, int interval, LogWriter& log) {
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
	wait.tv_sec = interval - (now.tv_sec%interval);
	
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
		
		wait.tv_sec = interval - (now.tv_sec%interval);
	}
	return 0;
}
