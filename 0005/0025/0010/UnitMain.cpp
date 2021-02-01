/////////////////////////////////////////////////////////////////////////////
//	Project Name
//	  CSP_CCS.091RC AND CSP_CCSU.05RC => CSP_CCSU.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: 	Complex Communication Scheme with Unification (CCSU) Testbed
//		Project 0005.0025.0008 and 0005.0010 update
//		S = {P1 || P2 || P3 || P4 || Q1 || Q2 || C}
//		0008.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 8.D. Exemplary sample system.png
//		Logs
//
//		csp namespac ver. 0.91RC
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
int _tmain(int argc, _TCHAR* argv[])
{
	////////////////////////////////////////////////////////////////////////////
	//  COMMUNICAIONS MEDIUM
	////////////////////////////////////////////////////////////////////////////
	//
	//  Channels
	auto chanP1Q1 = std::make_shared<CHAN>(L"ChanP1Q1");
	//
	auto chanP2Q2 = std::make_shared<CHAN>(L"ChanP2Q2");
	auto chanP3Q2 = std::make_shared<CHAN>(L"ChanP3Q2");
	auto chanP4Q2 = std::make_shared<CHAN>(L"ChanP4Q2");
	//
	auto chanQ1C = std::make_shared<CHAN>(L"ChanQ1C");
	auto chanQ2C = std::make_shared<CHAN>(L"ChanQ2C");
	//
	// Process Q1 input channels multiplex
	MUX_PTR muxInQ1 = std::make_shared<MUX>();
	muxInQ1->add(chanP1Q1);
	//
	// Process Q2 input channels multiplex
	MUX_PTR muxInQ2 = std::make_shared<MUX>();
	muxInQ2->add(chanP2Q2);
	muxInQ2->add(chanP3Q2);
	muxInQ2->add(chanP4Q2);
	//
	// Process C input channels multiplex
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanQ1C);
	muxInC->add(chanQ2C);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: 	Complex Communication Scheme with Unification (CCSU) Testbed" << std::endl;
	std::cout << std::endl << ">>> Test Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//  S = {P1 || P2 || P3 || P4 || Q1 || Q2 || C}
	//
	vThreads.push_back(std::thread(doP, chanP1Q1, 1));

	vThreads.push_back(std::thread(doP, chanP2Q2, 2));
	vThreads.push_back(std::thread(doP, chanP3Q2, 3));
	vThreads.push_back(std::thread(doP, chanP4Q2, 4));

	vThreads.push_back(std::thread(doQ, muxInQ1, chanQ1C, 1));
	vThreads.push_back(std::thread(doQ, muxInQ2, chanQ2C, 2));

	vThreads.push_back(std::thread(doC, muxInC));
	////////////////////////////////////////////////////////////////////////////

	for(auto& t: vThreads)
	{
		if(t.joinable())
		{
			t.join();
		}
	}

	std::cout << ">>> Test End >>>" << std::endl;

	// "Press any key to continue..."
	std::cout << std::endl << std::endl;
	system("pause");

	return 0;
}
//------------------------------------------------------------------------------
//  Main function of process P
//  P1 = P2 = P3 = P4 = P
void doP(CHAN_PTR out, int pid)
{
	auto repeat = 1;
	if (pid == 1)
	{   // P1
		repeat = 3;
	}

	for(auto i = 0; i < repeat * LIMIT; i++)
	{
		// P1 = {Q1 ! msg}
		// P2 = {Q2 ! msg}
		std::wstringstream wss;
		wss << L"<P" << std::to_wstring(pid) << L"::"
			<< std::setw(3) << std::setfill(L'0')
			<< std::to_wstring(i) << L">";

		MSG msg = wss.str();
		out->send(msg);

		// Delay to check for Race Conditions
//		std::this_thread::sleep_for(std::chrono::milliseconds(2));
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
//------------------------------------------------------------------------------
//  Main function of process Q
//  Q1 = Q2 = Q
void doQ(MUX_PTR in, CHAN_PTR out, int pid)
{
	auto repeat = 3;

	for(auto i = 0; i < repeat * LIMIT; i++)
	{
		// Q1 = {P1 ? dst}
		// Q2 = {P2 ? dst [] P3 ? dst [] P4 ? dst}
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
//		std::this_thread::sleep_for(std::chrono::milliseconds(2));
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
//------------------------------------------------------------------------------
//  Main function of process C
void doC(MUX_PTR in)
{
	try
	{
		auto tp = std::chrono::system_clock::now();
		auto tt = std::chrono::system_clock::to_time_t(tp);

		std::wstringstream fn;
		// https://en.cppreference.com/w/cpp/io/manip/put_time
		fn << std::put_time(std::localtime(&tt), L"..\\..\\Logs\\Log.%Y%m%d.%H%M%S.txt");
		std::wofstream wof(fn.str());

		for(auto i = 0; i < (3 + 3)*LIMIT; i++)
		{
			// {Q1 ? dst [] Q2 ? dst}
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
//------------------------------------------------------------------------------

