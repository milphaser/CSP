/////////////////////////////////////////////////////////////////////////////
//	Project Name
//    CSP_1P5Q1C.06RC => CSP_1P5Q1C.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//			Application: Output Fork - Input Mux Testbed
//			Project 0005.0012 update
//			S = {P || Q1 || Q2 || Q3 || Q4 || Q5 || C}
//			0012.png
//			"C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 10.D. Fork-Mux.png"
//			Logs
//
//			csp namespac ver. 0.91RC
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
	auto chanPQ1 = std::make_shared<CHAN>(L"ChanPQ1");
	auto chanPQ2 = std::make_shared<CHAN>(L"ChanPQ2");
	auto chanPQ3 = std::make_shared<CHAN>(L"ChanPQ3");
	auto chanPQ4 = std::make_shared<CHAN>(L"ChanPQ4");
	auto chanPQ5 = std::make_shared<CHAN>(L"ChanPQ5");
	//
	auto chanQ1C = std::make_shared<CHAN>(L"ChanQ1C");
	auto chanQ2C = std::make_shared<CHAN>(L"ChanQ2C");
	auto chanQ3C = std::make_shared<CHAN>(L"ChanQ3C");
	auto chanQ4C = std::make_shared<CHAN>(L"ChanQ4C");
	auto chanQ5C = std::make_shared<CHAN>(L"ChanQ5C");
	//
	// Process Q1 input channels multiplex setup
	MUX_PTR muxInQ1 = std::make_shared<MUX>();
	muxInQ1->add(chanPQ1);
	//
	// Process Q2 input channels multiplex setup
	MUX_PTR muxInQ2 = std::make_shared<MUX>();
	muxInQ2->add(chanPQ2);
	//
	// Process Q3 input channels multiplex setup
	MUX_PTR muxInQ3 = std::make_shared<MUX>();
	muxInQ3->add(chanPQ3);
	//
	// Process Q4 input channels multiplex setup
	MUX_PTR muxInQ4 = std::make_shared<MUX>();
	muxInQ4->add(chanPQ4);
	//
	// Process Q5 input channels multiplex setup
	MUX_PTR muxInQ5 = std::make_shared<MUX>();
	muxInQ5->add(chanPQ5);
	//
	// Process P output channels fork setup
	FORK_PTR forkOutP = std::make_shared<FORK>();
	forkOutP->add(chanPQ1);
	forkOutP->add(chanPQ2);
	forkOutP->add(chanPQ3);
	forkOutP->add(chanPQ4);
	forkOutP->add(chanPQ5);
	//
	// Process C input channels multiplex setup
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanQ1C);
	muxInC->add(chanQ2C);
	muxInC->add(chanQ3C);
	muxInC->add(chanQ4C);
	muxInC->add(chanQ5C);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Output Fork - Input Mux Testbed" << std::endl;
	std::cout << ">>> Test Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	S = {P || Q1 || Q2 || Q3 || Q4 || Q5 || C}
	//
	vThreads.push_back(std::thread(doP, forkOutP));

	vThreads.push_back(std::thread(doQ, muxInQ1, chanQ1C, 1));
	vThreads.push_back(std::thread(doQ, muxInQ2, chanQ2C, 2));
	vThreads.push_back(std::thread(doQ, muxInQ3, chanQ3C, 3));
	vThreads.push_back(std::thread(doQ, muxInQ4, chanQ4C, 4));
	vThreads.push_back(std::thread(doQ, muxInQ5, chanQ5C, 5));

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
void doP(FORK_PTR out)
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
//------------------------------------------------------------------------------
//  Main function of process Q
//  Q1 = Q2 = Q3 = Q4 = Q5 = Q
void doQ(MUX_PTR in, CHAN_PTR out, int pid)
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
//		std::this_thread::sleep_for(std::chrono::milliseconds(2));
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
//------------------------------------------------------------------------------
//  Main function of process C
//  print process
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
//------------------------------------------------------------------------------

