//---------------------------------------------------------------------------
#ifndef proactorH
#define proactorH
//---------------------------------------------------------------------------
#include <thread>
//---------------------------------------------------------------------------
template <class TArg1, class ...TArgs>
class proactor
{
public:
	proactor(void) : started(false) {}
	proactor(proactor const&) = delete;
	proactor& operator=(proactor const&) = delete;

	~proactor(void) 	{join();}   					//  implicitly join
	void join(void)     {if(t.joinable()) t.join();}    //  excplicitly join before destruction

protected:
	void start(TArg1 arg1, TArgs... args)
	{
		if(!started)
		{
			OnInit();
			started = true;
			t = std::move(std::thread([this, arg1, args...](){this->exec(arg1, args...);}));
		}
	}
	virtual void body(TArg1 arg1, TArgs... args) = 0;
	//  Event Handlers ---
	virtual void OnInit(void)   {}

private:
	bool started;
	std::thread t;

	void exec(TArg1 arg1, TArgs... args)
	{
		try
		{
			body(arg1, args...);
		}
		catch(...)
		{
			/* TODO : Add record to Log */
		}
	}
};
//---------------------------------------------------------------------------
#endif

