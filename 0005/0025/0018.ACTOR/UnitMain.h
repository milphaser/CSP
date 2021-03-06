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

using SINK = csp::sink<std::wstring, __MSG>;
using SINK_PTR = std::shared_ptr<SINK>;

using MSG = CHAN::MSG;
//---------------------------------------------------------------------------
const int LIMIT = 10;			// Number of working cycles
const bool RCC_DELAY = true;    // RCC delay control
const SINK::Sinking sinking = SINK::Sinking::by_place;
//const SINK::Sinking sinking = SINK::Sinking::parallel;
//---------------------------------------------------------------------------
extern std::wofstream wof;         // Log file
//---------------------------------------------------------------------------
//	Utilities
//  Race Conditions Check Delay
int rcc_delay(int pid)
{
	if(!RCC_DELAY)
	{
		return 0;
	}

	int dur = 0;

	switch(pid)
	{
		case 1:     // P1 delay, ms
			dur = 1500;
			break;
		case 2:     // P2 delay, ms
			dur = 500;
			break;
		case 3:     // P3 delay, ms
			dur = 1000;
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
//  P1 = P2 = P3 = P
class actorP: public proactor<CHAN_PTR>
{
public:
	actorP(CHAN_PTR out, int __pid): pid(__pid) {start(out);}

protected:
	void body(CHAN_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// Delay to check for Race Conditions
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(pid)));

			// P = {Q ! msg}
			std::wstringstream wss;
			wss << L"<P" << std::to_wstring(pid) << L"::"
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
class actorQ: public proactor<SINK_PTR>
{
public:
	actorQ(SINK_PTR in) {start(in);}

protected:
	void body(SINK_PTR in)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			auto tp1 = std::chrono::system_clock::now();    		// timepoint 1

			// Q = {{P1 ? dst[1] || P2 ? dst[2] ||P3 ? dst[3]}; print(dst);}
			std::vector<MSG> dst = in->recv(sinking);

			auto tp2 = std::chrono::system_clock::now();    		// timepoint 2
			auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();

			std::wstringstream wss;
			wss << L"Sink Delay=" + std::to_wstring(delay) + L" ms" << std::endl;

			for(auto pid = 0; pid < dst.size(); pid++)
			{
				wss << "#" << std::setw(3) << std::setfill(L'0') << std::to_wstring(i) << " "
					<< dst[pid] << L" <Q::"
					<< std::setw(3) << std::setfill(L'0')
					<< std::to_wstring(i) << L"::" << std::to_wstring(pid + 1) + L">"
					<< std::endl;
			}
			wss << L"+++\n";
			std::wcout << wss.str();
			wof << wss.str();
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
		CHAN_PTR outP1,
		CHAN_PTR outP2,
		CHAN_PTR outP3,
		SINK_PTR inQ
		)
	{
		aP1 = std::make_shared<actorP>(outP1, 1);
		aP2 = std::make_shared<actorP>(outP2, 2);
		aP3 = std::make_shared<actorP>(outP3, 3);
		aQ  = std::make_shared<actorQ>(inQ);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP1;		// Process P1
	std::shared_ptr<actorP> aP2;		// Process P2
	std::shared_ptr<actorP> aP3;		// Process P3
	std::shared_ptr<actorQ> aQ;			// Process Q

private:
};
//---------------------------------------------------------------------------
#endif

