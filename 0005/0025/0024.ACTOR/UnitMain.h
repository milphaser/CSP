//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
#include "proactor.h"
//---------------------------------------------------------------------------
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
//---------------------------------------------------------------------------
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
const bool ACCESS_DELAY = true;	// Exec delay control
//---------------------------------------------------------------------------
extern std::wofstream wof;         // Log file
extern int delays[];       	// Total delays to grant by process
//---------------------------------------------------------------------------
//	Utilities
void access(std::wstring msg, std::chrono::milliseconds duration);
int p_in_access(int pid)
{
	if(!ACCESS_DELAY)
	{
		return 0;
	}

	int dur = 0;

	switch(pid)
	{
		case 1:     // P1 in access, ms
			dur = 500;
			break;
		case 2:     // P2 in access, ms
			dur = 500;
			break;
		case 3:     // P3 in access, ms
			dur = 500;
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
class actorP: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP(CHAN_PTR in, CHAN_PTR out, int __pid): pid(__pid) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{C!<req> -> C?<grant> -> access(); C!rls}
		for(auto i = 0; i < LIMIT; i++)
		{
			// {C!req}
			auto tp1 = std::chrono::system_clock::now();    		// timepoint 1
			auto tt = std::chrono::system_clock::to_time_t(tp1);    // convert to a time_t

			std::wstringstream tstr;
			tstr << std::put_time(std::localtime(&tt), L"%H:%M:%S");

			msg = L"P" + std::to_wstring(pid) + L" requested control [Time(req)=" + tstr.str() + L"]";
			out->send(msg);     // send <req>

			// {C?<grant>}
			in->recv(msg);      // recv <grant>
			auto tp2 = std::chrono::system_clock::now();    		// timepoint 2
			auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
			delays[pid - 1] += delay;

			std::wstringstream wss;
			wss << L"<P" + std::to_wstring(pid) + L"::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [Delay(grant)=" + std::to_wstring(delay) + L" ms]"
				<< L" [T(access)=" + std::to_wstring(p_in_access(pid)) + L" ms]";
			access(wss.str(), std::chrono::milliseconds(p_in_access(pid)));

			// {C!<rls>}
			msg = L"P" + std::to_wstring(pid) + L" released control";
			out->send(msg);     // send <rls>
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  AO Controller/Dispatcher Class
class actorC: public proactor<MUX_PTR, FORK_PTR>
{
public:
	actorC(MUX_PTR in, FORK_PTR out) {start(in, out);}

protected:
	void body(MUX_PTR in, FORK_PTR out)	override
	{
		MSG msg;

		std::wcout << "+++" << "\n";
		wof << "+++" << std::endl;

		for(auto i = 0; i < LIMIT; i++)
		{
			//  Dispatching of Coprocedures
			for(auto repeat = 0; repeat < 3; repeat++)
			{
				// {Pid?<req>}
				auto pid = in->recv(msg);    // recv <req> non-deterministically
				std::wcout << msg << "\n";
				wof << msg << std::endl;

				// {Pid!<grant>}
				msg = L"C granted control to P" + std::to_wstring(pid + 1);
				std::wcout << msg << "\n";
				wof << msg << std::endl;
				out->send(msg, pid);         // send <grant>

				// {Pid?<rls>}
				in->recv(msg, pid);          // recv <rls>
				std::wcout << msg << "\n";
				wof << msg << std::endl;

				std::wcout << "+++" << "\n";
				wof << "+++" << std::endl;
			}
		}
	}

	//  Event Handlers ---
	void OnInit(void)	override
	{
	}

private:
};
//---------------------------------------------------------------------------
//  Class Parallel System
//  encapsulates all system actors
class parsys
{
public:
	parsys(
		CHAN_PTR inP1, CHAN_PTR outP1,
		CHAN_PTR inP2, CHAN_PTR outP2,
		CHAN_PTR inP3, CHAN_PTR outP3,
		MUX_PTR inC, FORK_PTR outC)
	{
		aP1 = std::make_shared<actorP>(inP1, outP1, 1);
		aP2 = std::make_shared<actorP>(inP2, outP2, 2);
		aP3 = std::make_shared<actorP>(inP3, outP3, 3);
		aC = std::make_shared<actorC>(inC, outC);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//	AO Controller (C) should be explicitly joined before destruction
		aC->join();
		//  P1, P2 and P3 will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP1;	// Process P1
	std::shared_ptr<actorP> aP2;	// Process P2
	std::shared_ptr<actorP> aP3;	// Process P3
	std::shared_ptr<actorC> aC;		// AO Controller/Dispatcher

private:
};
//---------------------------------------------------------------------------
#endif

