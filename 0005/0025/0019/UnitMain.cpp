/////////////////////////////////////////////////////////////////////////////
//	Project Name
//      CSP_3P1Q_sink.08RC => CSP_Sink_Mux.08RC => CSP_Sink_Mux.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Sink-Mux Exemplary System
//		Project 005.0019 update
//
//		 S = {S1 || S2 || C}
//		S1 = {P1 || P2 || P3 || Q1}
//		S2 = {P4 || P5 || P6 || Q2}
//		Q1 = {{P1 ? dst[1] || P2 ? dst[2] ||P3 ? dst[3]}; C ! dst}
//		Q2 = {{P4 ? dst[1] || P5 ? dst[2] ||P6 ? dst[3]}; C ! dst}
//		 C = {{Q1 ? var [] Q2 ? var}; print(var);}
//
//		0019.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.1.3. Sink-Mux Exemplary System.png
//		Logs
//
//		[csp namespac ver. 0.91RC]
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
	auto chanP1Q1 = std::make_shared<CHAN>(L"P1Q1");
	auto chanP2Q1 = std::make_shared<CHAN>(L"P2Q1");
	auto chanP3Q1 = std::make_shared<CHAN>(L"P3Q1");
	//
	auto chanP4Q2 = std::make_shared<CHAN>(L"P4Q2");
	auto chanP5Q2 = std::make_shared<CHAN>(L"P5Q2");
	auto chanP6Q2 = std::make_shared<CHAN>(L"P6Q2");
	//
	auto chanQ1C = std::make_shared<CHAN>(L"Q1C");
	auto chanQ2C = std::make_shared<CHAN>(L"Q2C");
	//
	// Process Q1 input channels sink setup
	SINK_PTR sinkInQ1 = std::make_shared<SINK>();
	sinkInQ1->add(chanP1Q1);
	sinkInQ1->add(chanP2Q1);
	sinkInQ1->add(chanP3Q1);
	//
	// Process Q2 input channels sink setup
	SINK_PTR sinkInQ2 = std::make_shared<SINK>();
	sinkInQ2->add(chanP4Q2);
	sinkInQ2->add(chanP5Q2);
	sinkInQ2->add(chanP6Q2);
	//
	// Process C input channels multiplex setup
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanQ1C);
	muxInC->add(chanQ2C);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Sink-Mux Exemplary System" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;

	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	 S = {S1 || S2 || C}
	//	S1 = {P1 || P2 || P3 || Q1}
	//	S2 = {P4 || P5 || P6 || Q2}
	//
	vThreads.push_back(std::thread(doP, chanP1Q1, 1));
	vThreads.push_back(std::thread(doP, chanP2Q1, 2));
	vThreads.push_back(std::thread(doP, chanP3Q1, 3));

	vThreads.push_back(std::thread(doP, chanP4Q2, 4));
	vThreads.push_back(std::thread(doP, chanP5Q2, 5));
	vThreads.push_back(std::thread(doP, chanP6Q2, 6));

	vThreads.push_back(std::thread(doQ, sinkInQ1, chanQ1C, 1));
	vThreads.push_back(std::thread(doQ, sinkInQ2, chanQ2C, 2));

	vThreads.push_back(std::thread(doC, muxInC));
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
		wss << L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl;
		if(sinking == SINK::Sinking::by_place)
		{
			wss << L"Sinking: by_place" << std::endl;
		}
		else if(sinking == SINK::Sinking::parallel)
		{
			wss << L"Sinking: parallel (default)" << std::endl;
		}

		std::wcout << wss.str();
		wof << wss.str();
	}
	//
	for(auto i = 0; i < 6; i++)
	{
		std::wstringstream wss;
		wss << L"RCC Delay P" << std::to_wstring(i+1)
			<< L": " << std::setw(4) << std::to_wstring(rcc_delay(i+1))
			<< L" ms" << std::endl;

		std::wcout << wss.str();
		wof << wss.str();
	}
	////////////////////////////////////////////////////////////////////////////

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
//  P1 = P2 = P3 = P4 = P5 = P6 = P
void doP(CHAN_PTR out, int pid)
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
//------------------------------------------------------------------------------
//  Main function of process Q
//  Q1 = Q2 = Q
void doQ(SINK_PTR in, CHAN_PTR out, int pid)
{
	for(auto i = 0; i < LIMIT; i++)
	{
		// Q1 = {{P1 ? dst[1] || P2 ? dst[2] ||P3 ? dst[3]}; print(dst);}
		// Q2 = {{P4 ? dst[1] || P5 ? dst[2] ||P6 ? dst[3]}; print(dst);}
		std::vector<MSG> dst = in->recv(sinking);

		std::wstringstream wss;
		for(auto k = 0; k < dst.size(); k++)
		{
			wss << "#" << std::setw(3) << std::setfill(L'0') << std::to_wstring(i) << " "
				<< dst[k]
				<< L" <Q" << std::to_wstring(pid) << L"::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) << L"::" << std::to_wstring(k + 1) + L">"
				<< std::endl;
		}
		wss << L"+++\n";

		MSG msg = wss.str();
		out->send(msg);
	}
}
//------------------------------------------------------------------------------
//  Main function of process C
//
void doC(MUX_PTR in)
{
	for(auto i = 0; i < 2*LIMIT; i++)
	{
		//	C = {{Q1 ? var [] Q2 ? var}; print(var);}
		MSG var;
		int id = in->recv(var);

		std::wstringstream wss;
		wss << "Channel #" << std::to_wstring(id) << std::endl
			<< var << std::endl;

		std::wcout << wss.str();
		wof << wss.str();
	}
}
//------------------------------------------------------------------------------

