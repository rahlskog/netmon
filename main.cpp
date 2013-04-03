#include <iostream>
#include <fstream>
#include <stdint.h>
#include <time.h>

using std::cout;
using std::endl;

const int run_interval = 60;

void Usage(std::string name);
int MainLoop(std::string interface);
bool GetBytes(std::string source, uint64_t& bytes);

int main(int argc, char **argv) {
	if (argc < 2) {
		Usage(argv[0]);
		return -1;
	}
	
	std::ifstream ifile(std::string("/sys/class/net/")+argv[1]);
	if (!ifile)
	{
		cout << "Interface \"" << argv[1] << "\" not found" << endl;
		Usage(argv[0]);
		return -1;
	}
	ifile.close();
	
	return MainLoop(argv[1]);
}

void Usage(std::string name) {
	cout << "Usage:\n  " << name << " [interface]\n" << endl;
}

int MainLoop(std::string interface) {
	std::string RX("/sys/class/net/"+interface+"/statistics/rx_bytes");
	std::string TX("/sys/class/net/"+interface+"/statistics/tx_bytes");
	
	std::ifstream ifile(interface+".log");
	if (!ifile)
	{
		std::ofstream ofile(interface+".log");
		ofile << "#timestamp,time-delta,rx-delta,tx-delta" << endl;
	}
	ifile.close();
	
	
	uint64_t rx_last, rx_now;
	uint64_t tx_last, tx_now;
	timespec now{0,0};
	timespec wait{0,0};
	timespec remain{0,0};
	
	if (!(GetBytes(RX, rx_last) && GetBytes(TX, tx_last))) {
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

		if (!(GetBytes(RX, rx_now) && GetBytes(TX, tx_now))) {
			cout << "Failed reading current valuies" << endl;
			return -1;
		}
		
		std::ofstream log(interface+".log", std::ios::app);
		
		log << now.tv_sec << "," << wait.tv_sec << ",";
		log << rx_now - rx_last << "," << tx_now - tx_last << endl;
		
		log.close();
		
		rx_last = rx_now;
		tx_last = tx_now;
		
		wait.tv_sec = run_interval - (now.tv_sec%run_interval);
	}
	return 0;
}

bool GetBytes(std::string source, uint64_t &bytes) {
	std::ifstream src(source);
	src >> bytes;
	return src.good();
}
