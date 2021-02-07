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
//  Buffer
//  Q1 = Q2 = Q3 = Q4 = Q5 = Q
class actorQ: public proactor<MUX_PTR, CHAN_PTR, int>
{
public:
	actorQ(MUX_PTR in, CHAN_PTR out, int pid) {start(in, out, pid);}

protected:
	void body(MUX_PTR in, CHAN_PTR out, int pid)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// Qi = {P ? dst -> C ! dst}
			MSG dst;
			int id = in->recv(dst);  // alternative command

			// {C ! msg}
			std::wstringstream wss;
			wss << dst << L" <Q" << std::to_wstring(pid) << L"::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) << L"::" << std::to_wstring(id + 1) << L">";

			MSG msg = wss.str();
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process C Class
//  Consumer
class actorC: public proactor<MUX_PTR>
{
public:
	actorC(MUX_PTR in) {start(in);}

protected:
	void body(MUX_PTR in)	override
	{
		try
		{
			auto tp = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(tp);

			std::wstringstream fn;
			// https://en.cppreference.com/w/cpp/io/manip/put_time
			fn << std::put_time(std::localtime(&tt), L"..\\..\\Logs\\Log.%Y%m%d.%H%M%S.txt");
			std::wofstream wof(fn.str());

			for(int i = 0; i < 5*LIMIT; i++)
			{
				// {Q1 ? dst [] Q2 ? dst [] Q3 ? dst [] Q4 ? dst [] Q5 ? dst}
				MSG dst;
				int id = in->recv(dst);  // alternative command

				std::wstringstream wss;
				wss << "#" << std::setw(3) << std::setfill(L'0') << std::to_wstring(i) << " "
					<< dst << L" <C::"
					<< std::setw(3) << std::setfill(L'0')
					<< std::to_wstring(i) << L"::" << std::to_wstring(id + 1) + L">"
					<< std::endl;
				std::wstring ws = wss.str();

				std::wcout << ws;
				wof << ws;
			}

			std::wcout << L"The Log saved to <" << fn.str() << L">" << std::endl;
		}
		catch(std::exception& e)
		{
			e.what();
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
		MUX_PTR inQ1, CHAN_PTR outQ1, int pidQ1,
		MUX_PTR inQ2, CHAN_PTR outQ2, int pidQ2,
		MUX_PTR inQ3, CHAN_PTR outQ3, int pidQ3,
		MUX_PTR inQ4, CHAN_PTR outQ4, int pidQ4,
		MUX_PTR inQ5, CHAN_PTR outQ5, int pidQ5,
		MUX_PTR inC
		)
	{
		aP  = std::make_shared<actorP>(outP);
		aQ1 = std::make_shared<actorQ>(inQ1, outQ1, pidQ1);
		aQ2 = std::make_shared<actorQ>(inQ2, outQ2, pidQ2);
		aQ3 = std::make_shared<actorQ>(inQ3, outQ3, pidQ3);
		aQ4 = std::make_shared<actorQ>(inQ4, outQ4, pidQ4);
		aQ5 = std::make_shared<actorQ>(inQ5, outQ5, pidQ5);
		aC  = std::make_shared<actorC>(inC);
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
	std::shared_ptr<actorC> aC;		// Process C

private:
};
//---------------------------------------------------------------------------
#endif

