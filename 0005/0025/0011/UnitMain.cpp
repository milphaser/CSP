/////////////////////////////////////////////////////////////////////////////
//	Project Name
//	  CSP_1P5Q.06RC => CSP_1P5Q.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Output Fork Testbed
//		Project 0005.0011 update
//		S = {P || Q1 || Q2 || Q3 || Q4 || Q5}
//		0011.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 9.D. Fork.png
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
std::wofstream wof;         // Log file
std::mutex mu_print;   		// Shared print mutex
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
	auto chanPQ1 = std::make_shared<CHAN>(L"ChanPQ1");
	auto chanPQ2 = std::make_shared<CHAN>(L"ChanPQ2");
	auto chanPQ3 = std::make_shared<CHAN>(L"ChanPQ3");
	auto chanPQ4 = std::make_shared<CHAN>(L"ChanPQ4");
	auto chanPQ5 = std::make_shared<CHAN>(L"ChanPQ5");
	//
	// Process P output channels fork setup
	FORK_PTR forkOutP = std::make_shared<FORK>();
	forkOutP->add(chanPQ1);
	forkOutP->add(chanPQ2);
	forkOutP->add(chanPQ3);
	forkOutP->add(chanPQ4);
	forkOutP->add(chanPQ5);
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
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: 	Output Fork Testbed" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//  S = {P || Q1 || Q2 || Q3 || Q4 || Q5}
	//
	vThreads.push_back(std::thread(doP, forkOutP));

	vThreads.push_back(std::thread(doQ, muxInQ1, 1));
	vThreads.push_back(std::thread(doQ, muxInQ2, 2));
	vThreads.push_back(std::thread(doQ, muxInQ3, 3));
	vThreads.push_back(std::thread(doQ, muxInQ4, 4));
	vThreads.push_back(std::thread(doQ, muxInQ5, 5));
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
void doQ(MUX_PTR in, int pid)
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
//------------------------------------------------------------------------------
void shared_print(const std::wstring& msg)
{
	std::lock_guard<std::mutex> guard(mu_print);   // RAII
	std::wcout << msg << std::endl;
	try
	{
		wof << msg << std::endl;
	}
	catch(std::exception& e)
	{
		e.what();
	}
}
//------------------------------------------------------------------------------

