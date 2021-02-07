//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
#include "proactor.h"
//---------------------------------------------------------------------------
#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <thread>
#include <ctime>
#include <chrono>
//------------------------------------------------------------------------------
using __MSG = std::wstring;    // data

using CHAN = csp::chan<std::wstring, __MSG>;
using CHAN_PTR = std::shared_ptr<CHAN>;

using MUX = csp::mux<std::wstring, __MSG>;
using MUX_PTR = std::shared_ptr<MUX>;

using FORK = csp::fork<std::wstring, __MSG>;
using FORK_PTR = std::shared_ptr<FORK>;

using MSG = CHAN::MSG;
//---------------------------------------------------------------------------
const int LIMIT = 10;			// Number of working cycles
//---------------------------------------------------------------------------
//	Utilities
void shared_print(const std::wstring& msg);
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
//  Producer
class actorP: public proactor<FORK_PTR>
{
public:
	actorP(FORK_PTR out) {start(out);}

protected:
	void body(FORK_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// {Q1!msg || Q2!msg || Q3!msg || Q4!msg || Q5!msg}
			std::wstringstream wss;
			wss << L"<P::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) << L">";

			MSG msg = wss.str();
			out->send(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q Class
//  Consumer
//  Q1 = Q2 = Q3 = Q4 = Q5 = Q
class actorQ: public proactor<MUX_PTR, int>
{
public:
	actorQ(MUX_PTR in, int pid) {start(in, pid);}

protected:
	void body(MUX_PTR in, int pid)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// Qi = {P ? dst -> print}
			MSG dst;
			auto id = in->recv(dst);  // alternative command

			// {C ! msg}
			std::wstringstream wss;
			wss << dst << L" <Q" << std::to_wstring(pid) << L"::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) << L"::" << std::to_wstring(id + 1) << L">";

			MSG msg = wss.str();
			shared_print(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Class Parallel System
//  encapsulates all system actors
class parsys
{
public:
	parsys(
		FORK_PTR outP,
		MUX_PTR inQ1, int pidQ1,
		MUX_PTR inQ2, int pidQ2,
		MUX_PTR inQ3, int pidQ3,
		MUX_PTR inQ4, int pidQ4,
		MUX_PTR inQ5, int pidQ5
		)
	{
		aP  = std::make_shared<actorP>(outP);
		aQ1 = std::make_shared<actorQ>(inQ1, pidQ1);
		aQ2 = std::make_shared<actorQ>(inQ2, pidQ2);
		aQ3 = std::make_shared<actorQ>(inQ3, pidQ3);
		aQ4 = std::make_shared<actorQ>(inQ4, pidQ4);
		aQ5 = std::make_shared<actorQ>(inQ5, pidQ5);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP;		// Process P
	std::shared_ptr<actorQ> aQ1;	// Process Q1
	std::shared_ptr<actorQ> aQ2;	// Process Q2
	std::shared_ptr<actorQ> aQ3;	// Process Q3
	std::shared_ptr<actorQ> aQ4;	// Process Q4
	std::shared_ptr<actorQ> aQ5;	// Process Q5

private:
};
//---------------------------------------------------------------------------
#endif

