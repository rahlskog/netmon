#include "builtins.h"
#include <fstream>

SysfsReader::SysfsReader(std::string IfName) : StatReader(IfName)
{
    rx_src = ("/sys/class/net/"+IfName+"/statistics/rx_bytes");
    tx_src = ("/sys/class/net/"+IfName+"/statistics/tx_bytes");
    rx_now = tx_now = 0;
    rx_last = tx_last = 0;

    std::ifstream test(rx_src);
    if (!test)
        throw InterfaceException("Could not open interface: " + IfName);
    test.close();
}
bool SysfsReader::Update() {
    rx_last = rx_now;
    tx_last = tx_now;
    if (!GetBytes(rx_src, rx_now)) return false;
    if (!GetBytes(tx_src, tx_now)) return false;
    return true;
}
int SysfsReader::RxDelta() const {
    return rx_now - rx_last;
}
int SysfsReader::TxDelta() const {
    return tx_now -tx_last;
}
bool SysfsReader::GetBytes(std::string source, uint64_t& bytes) {
    std::ifstream src(source);
    src >> bytes;
    return src.good();
}

FileWriter::FileWriter()
{
}
bool FileWriter::Log(LogPackage lp)
{
	std::string fName = lp.IfName+".log";
	std::ofstream log(fName, std::ios::app);
	if (log.tellp() <= 0)
	{
		log << "#timestamp,time-delta,rx-delta,tx-delta" << std::endl;
	}
	log << lp.Time << "," << lp.TimeDelta << "," << lp.RxDelta << "," << lp.TxDelta << std::endl;
	log.flush();
	log.close();
	
	return true;
}

