/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_3P1C_ACTOR_DME.091RC
//		[CSP_3P1C_DME_Var2.07RC + CSP_ACTOR_MB.091RC => CSP_3P1C_ACTOR_DME.091RC]
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Actor Implementation of DME Var2
//		Project 0005.0016 [CSP_3P1C_DME_Var2.07RC] implemented as Actor,
//		following Project 0005.0023 [CSP_ACTOR_MB.091RC]
//
//		S = {P1 || P2 || P3 || C}
//		0015.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.6. DME.png
//
//		[csp namespace ver. 0.91RC]
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
#include <sstream>
//------------------------------------------------------------------------------
std::wofstream wof;         // Log file
int delays[3];          	// Total delays to grant by process
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
	forkOutC->add(chanCP3);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Distributed Mutual Exclusion with Central Server" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
//		S = {P1 || P2 || P3 || C}
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

	////////////////////////////////////////////////////////////////////////////
	//  Run Parameters/Report
	//
	{
		std::wstringstream wss;
		wss << "+++" << std::endl
			<< L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl;

		std::wcout << wss.str();
		wof << wss.str();
	}
	//
	{
		std::wstringstream wss;
		for(auto i = 0; i < 3; i++)
		{
			wss << L"Average Delay(grant) P" << std::to_wstring(i+1)
				<< L": " << std::setw(4) << std::to_wstring(delays[i]/LIMIT)
				<< L" ms" << std::endl;
		}
		wss << "+++" << std::endl;

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
void access(std::wstring msg, std::chrono::milliseconds duration)
{
	// Access to shared object
	std::wcout << msg << "\n";
	wof << msg << std::endl;
	std::this_thread::sleep_for(duration);
}
//------------------------------------------------------------------------------

