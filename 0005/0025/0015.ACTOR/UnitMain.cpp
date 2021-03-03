/////////////////////////////////////////////////////////////////////////////
//	Project Name
//      CSP_3P1C_DME_CoP.07RC => CSP_3P1C_DME_CoP.091RC => CSP_ACTOR_3P1C_DME_CoP.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Distributed Mutual Exclusion with Central Server (DME)
//		(*) Var1:	Centralized control of coprocedures
//			   		Strong predefined sequential dispatching
//		As a protected shared object is used std::wcout
//
//		Manifold usage:
//		- coprocedures dispatching with central server (Var1)
//		- analogue to distributed mutual exclusion with central server (Var2)
//		S = {P1 || P2 || P3 || C}
//		0015.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.6. DME.png
//      Logs
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
std::wofstream wof;
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
	auto chanP1C = std::make_shared<CHAN>(L"ChanP1C");
	auto chanP2C = std::make_shared<CHAN>(L"ChanP2C");
	auto chanP3C = std::make_shared<CHAN>(L"ChanP3C");
	//
	auto chanCP1 = std::make_shared<CHAN>(L"ChanCP1");
	auto chanCP2 = std::make_shared<CHAN>(L"ChanCP2");
	auto chanCP3 = std::make_shared<CHAN>(L"ChanCP3");
	//
	// Process C input channels multiplex setup
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanP1C);
	muxInC->add(chanP2C);
	muxInC->add(chanP3C);
	//
	// Process C output channels fork setup
	FORK_PTR forkOutC = std::make_shared<FORK>();
	forkOutC->add(chanCP1);
	forkOutC->add(chanCP2);
	forkOutC->
	add(chanCP3);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Distributed Mutual Exclusion with Central Server" << std::endl;
	std::cout << ">>> (*) Var1: \tCentralized control of coprocedures" << std::endl;
	std::cout << ">>> \t\tStrong predefined sequential dispatching" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//	S = {P1 || P2 || P3 || C}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	{
		parsys parSys(
			   chanCP1, chanP1C,		// actorP1
			   chanCP2, chanP2C,		// actorP2
			   chanCP3, chanP3C,		// actorP3
			   muxInC, forkOutC);   	// actorC (AO Controller/Dispatcher)
	}   // 	RAII => join
	//
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

