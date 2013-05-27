#ifndef INTERFACES_H
#define INTERFACES_H
#include <stdexcept>
#include <string>

class InterfaceException : public std::runtime_error {
public:
    InterfaceException(std::string wh) : std::runtime_error(wh) {}
};

class StatReader {
public:
	StatReader(std::string IfName) : ifname(IfName) {}
	virtual ~StatReader() {}
	virtual bool Update() = 0;
	virtual int RxDelta() const = 0;
	virtual int TxDelta() const = 0;
	std::string IfName() const { return ifname; }
private:
	std::string ifname;
};

class LogPackage {
public:
	std::string IfName;
	time_t Time;
	time_t TimeDelta;
	unsigned int RxDelta;
	unsigned int TxDelta;
};

class LogWriter {
public:
	LogWriter() {}
	virtual ~LogWriter() {}
	virtual bool Log(LogPackage lp) = 0;
};

#endif //INTERFACES_H