/////////////////////////////////////////////////////////////////////////////
//	Project Name
//    CSP_1P5Q1C.06RC => CSP_1P5Q1C.091RC => CSP_ACTOR_1P5Q1C.091RC
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
	//
	//	S = {P || Q1 || Q2 || Q3 || Q4 || Q5 || C}
	//
	{
		parsys parSys
		(
			forkOutP,				// actorP
			muxInQ1, chanQ1C, 1,	// actorQ
			muxInQ2, chanQ2C, 2,	// actorQ
			muxInQ3, chanQ3C, 3,	// actorQ
			muxInQ4, chanQ4C, 4,	// actorQ
			muxInQ5, chanQ5C, 5,	// actorQ
			muxInC					// actorC
		);
	}   // 	RAII => join
	//
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Test End >>>" << std::endl;

	// "Press any key to continue..."
	std::cout << std::endl << std::endl;
	system("pause");

	return 0;
}
//------------------------------------------------------------------------------

