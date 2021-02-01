/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_2P1Q.04 => CSP_2P1Q.07RC => CSP_2P1Q.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Producer-Customer Type 2:1 (Non-determinism)
//		Project 0005.00017.0007 update
//		{P1 || Q || P2}
//		0017.0007.1.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 4. 2-channel non-deterministic selection.png
//		0017.0007.2.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.1. mux (n to 1).png
//
//		csp namespace ver. 0.91RC >>>
//	Status: Completed
/////////////////////////////////////////////////////////////////////////////
#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include "UnitMain.h"
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
std::wofstream wof;         // Log file
//------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	auto tp = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(tp);

	std::wstringstream fn;
	// https://en.cppreference.com/w/cpp/io/manip/put_time
	fn << std::put_time(std::localtime(&tt), L"..\\..\\Logs\\Log.%Y%m%d.%H%M%S.txt");
	wof.open(fn.str());

	////////////////////////////////////////////////////////////////////////////
	//  COMMUNICAIONS MEDIUM
	////////////////////////////////////////////////////////////////////////////
	//
	//  Channels
	auto chanP1Q = std::make_shared<CHAN>(L"ChanP1Q");
	auto chanP2Q = std::make_shared<CHAN>(L"ChanP2Q");
	//
	// Process Q input channels multiplex setup
	MUX_PTR muxInQ = std::make_shared<MUX>();
	muxInQ->add(chanP1Q);
	muxInQ->add(chanP2Q);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Producer-Customer Type 2:1 (Non-determinism)" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	S = {P1 || Q || P2}
	//
	vThreads.push_back(std::thread(doP, chanP1Q, 1));
	vThreads.push_back(std::thread(doP, chanP2Q, 2));
	vThreads.push_back(std::thread(doQ, muxInQ));
	////////////////////////////////////////////////////////////////////////////

	for(auto& t: vThreads)
	{
		if(t.joinable())
		{
			t.join();
		}
	}

	if(wof.good())
	{
		std::wcout << L"The Log saved to <" << fn.str() << L">" << std::endl;
	}
	wof.close();
	std::cout << ">>> End <<<" << std::endl;

	// "Press any key to continue..."
	std::cout << std::endl << std::endl;
	system("pause");

	return 0;
}
//------------------------------------------------------------------------------
//  Main function of process P
//  Producer
//  P1 = P2 = P
void doP(CHAN_PTR out, int pid)
{
	MSG msg;

	for(auto i = 0; i < LIMIT; i++)
	{
		// P = {Q ! <i>}
		msg = std::to_wstring(i);
		out->send(msg);

		// Delay to check for Race Conditions
//		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
//------------------------------------------------------------------------------
//  Main function of process Q
//  Consumer (Non-deterministic)
void doQ(MUX_PTR in)
{
	MSG var;

	for(auto i = 0; i < 2*LIMIT; i++)
	{
		// {P1 ? var [] P2 ? var}
//		int id = csp::alt(v_in, var, mux, csp::Dispatching::by_place) + 1;
		int id = in->recv(var);

		std::wstringstream wss;
		wss << "Data received: " << std::to_wstring(id) << "::" << var << "\n";
		std::wcout << wss.str();
		wof << wss.str();
	}
}
//------------------------------------------------------------------------------

