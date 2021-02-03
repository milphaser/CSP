/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_PBC_1_1_1.01 => CSP_1P1Q1C.07RC => CSP_1P1Q1C.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Producer-Buffer-Customer Type 1:1:1
//		Project 0005.0017.0004 update
//		{P || Q || C}
//		0017.0004.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 2.2. Pair of communicating processes with buffering.png
//
//		csp namespace ver. 0.091RC >>>
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
	auto chanPQ = std::make_shared<CHAN>(L"ChanPQ");
	auto chanQC = std::make_shared<CHAN>(L"ChanQC");
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Producer-Buffer-Customer Type 1:1:1" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	S = {P || Q}
	//
	vThreads.push_back(std::thread(doP, chanPQ));
	vThreads.push_back(std::thread(doQ, chanPQ, chanQC));
	vThreads.push_back(std::thread(doC, chanQC));
	////////////////////////////////////////////////////////////////////////////

	for(auto& t: vThreads)
	{
		if(t.joinable())
		{
			t.join();
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//  Run Parameters/Report
	//
	{
		std::wstringstream wss;
		wss << "+++" << std::endl
			<< L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl
			<< "Scenario #" << SCENARIO << std::endl;
		if(SYNC == CHAN::Sync::unilateral)
		{
			wss << "Unilateral (not CSP) synchronization" << std::endl;
		}
		else if(SYNC == CHAN::Sync::bilateral)
		{
			wss << "Bilateral (default) synchronization" << std::endl;
		}

		std::wcout << wss.str();
		wof << wss.str();
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
void doP(CHAN_PTR out)
{
	MSG msg;

	for(auto i = 0; i < LIMIT; i++)
	{
		// P = {Q ! i}
		msg = std::to_wstring(i);
		out->send(msg);

		// Delay to check for Race Conditions
		std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(1)));
	}
}
//------------------------------------------------------------------------------
//  Main function of process Q
//  Buffer
void doQ(CHAN_PTR in, CHAN_PTR out)
{
	MSG var;

	for(auto i = 0; i < LIMIT; i++)
	{
		// {P ? var}
		in->recv(var);
		// {C ! var}
		out->send(var);

		// Delay to check for Race Conditions
		std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(2)));
	}
}
//------------------------------------------------------------------------------
//  Main function of process C
//  Consumer
void doC(CHAN_PTR in)
{
	MSG var;

	for(auto i = 0; i < LIMIT; i++)
	{
		// C = {Q ? var}
		in->recv(var);

		std::wstringstream wss;
		wss << "Data received: " << var << std::endl;
		std::wcout << wss.str();
		wof << wss.str();
	}
}
//------------------------------------------------------------------------------

