#ifndef INTERFACES_H
#define INTERFACES_H
#include <stdexcept>

class Interface {
public:
	Interface(std::string IfName)
		: ifname(IfName)
	{}
	virtual bool Update() = 0;
	virtual int RxDelta() const = 0;
	virtual int TxDelta() const = 0;
	std::string IfName() const { return ifname; }
private:
	std::string ifname;
};

class InterfaceException : public std::runtime_error {
public:
    InterfaceException(std::string wh) : std::runtime_error(wh) {}
};

class SysfsInterface : public Interface {
public:
    SysfsInterface(std::string IfName)
		: Interface(IfName)
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
	bool Update()
	{
		rx_last = rx_now;
		tx_last = tx_now;
		if (!GetBytes(rx_src, rx_now)) return false;
		if (!GetBytes(tx_src, tx_now)) return false;
		return true;
	}
	int RxDelta() const
	{
		return rx_now - rx_last;
	}
	int TxDelta() const
	{
		return tx_now -tx_last;
	}
private:
	bool GetBytes(std::string source, uint64_t &bytes)
	{
		std::ifstream src(source);
		src >> bytes;
		return src.good();
	}
	std::string rx_src, tx_src;
	uint64_t rx_now, rx_last;
	uint64_t tx_now, tx_last;
};

#endif //INTERFACES_H