/*
 * Copyright (C) 2013 Robert Ahlskog.
 * 
 * This file is part of netmon, distributed under GNU GPL v3.
 * For full terms see included LICENSE file.
 */
#ifndef BUILTINS_H
#define BUILTINS_H
#include "interfaces.h"

class SysfsReader : public StatReader {
public:
	SysfsReader(std::string IfName);
	bool Update();
	int RxDelta() const;
	int TxDelta() const;
private:
	bool GetBytes(std::string source, uint64_t &bytes);
	std::string rx_src, tx_src;
	uint64_t rx_now, rx_last;
	uint64_t tx_now, tx_last;
};

class FileWriter : public LogWriter {
public:
	FileWriter();
	virtual bool Log(LogPackage lp);
};

#endif //BUILTINS_H
