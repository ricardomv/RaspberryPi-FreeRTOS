#include <video.h>
int depth = 0;
int intflag;

//use the GCC flag -finstrument-functions to enable tracing
//these trace functions will output the pointer to
//the function in which they are called by gcc
//to find the function name, look at the kernel.map

//you must add __attribute__((no_instrument_function))
//to any function you do not want traced
//especially ones which are printing out the trace :)

__attribute__((no_instrument_function))
void __cyg_profile_func_enter (void *this_fn, void *call_site){
	if(loaded == 2){
		depth++;
		char m[depth + 1];
		for(int i = 0; i < depth; i++){
			m[i] = ' ';
		}
		m[depth + 0] = 0; //null termination

		__asm volatile("push {r0} \n MRS r0, cpsr");
		register int *foo asm ("r0");
		int asdf = (int)foo;
		__asm volatile("pop {r0}");
		if((asdf & 0x80) && (depth < intflag)){
			m[0] = '#';
			printHex(m, (int)this_fn, 0xFF007F00); //red for call
			if(!(asdf & 0x80)) intflag = depth;
			return;
		}

		m[0] = '.';
		printHex(m, (int)this_fn, 0xFF00FF00); //green for call
		if(!(asdf & 0x80)) intflag = depth;
	}
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit  (void *this_fn, void *call_site){
	if(loaded == 2){
		char m[depth + 1];
		for(int i = 0; i < depth; i++){
			m[i] = ' ';
		}
		m[depth + 0] = 0; //null termination

		__asm volatile("push {r0} \n MRS r0, cpsr");
		register int *foo asm ("r0");
		int asdf = (int)foo;
		__asm volatile("pop {r0}");
		if((asdf & 0x80) && (depth < intflag)){
			m[0] = '#';
			printHex(m, (int)this_fn, 0xFF7F0000); //red for return
			depth--;
			return;
		}

		m[0] = '.';
		printHex(m, (int)this_fn, 0xFFFF0000); //red for return
		depth--;
	}
}