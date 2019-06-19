/*
 * hook.h
 * Utility class to add the ability to attach a hook to an event in another class
 * Rev 1 - 31.01.2014 - Keegan Morrow
 * 
 */

#ifndef __hook_h_
#define __hook_h_

#define HOOK 1 // revision number

/**
 * Utility class providing inheritable methods to implement hooks.
 * @author Keegan Morrow
 * @version 1 31.01.2014
 */
class hook
{
private:
	void (*eventHook)(void);
protected:
	/** 
	 * Calles the hooked function if there is one.
	 * This should be placed in the function in the inheriting class to call the hook.
	 */
	inline void callHook()
	{
		if (eventHook != NULL)
			(*eventHook)();
	}
public:
	hook()
	{
		eventHook = NULL;
	}
	/**
	 * Attach the function to be called.
	 * @param eventHook Function pointer to the function to be attached. In the form void foo().
	 */
	void attachHook(void (*eventHook)(void)) // pointer to a void foo() function -> bar.attachHook(foo);
	{
		this->eventHook = eventHook;
	}
	/**
	 * Detach the hook.
	 */
	void detachHook()
	{
		eventHook = NULL;
	}
};

#endif //__hook_h_
