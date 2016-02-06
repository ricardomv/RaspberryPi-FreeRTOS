#include <video.h>
int depth = 0;

//use the GCC flag -finstrument-functions to enable tracing
//these trace functions will output the pointer to
//the function in which they are called by gcc
//to find the function name, look at the kernel.map

//you must add __attribute__((no_instrument_function))
//to any function you do not want traced
//especially ones which are printing out the trace :)

__attribute__((no_instrument_function))
void __cyg_profile_func_enter (void *this_fn, void *call_site){
	if(loaded == 1){
		depth++;
		char m[depth + 1];
		for(int i = 0; i < depth; i++){
			m[i] = ' ';
		}
		m[depth + 0] = 0; //null termination
		m[0] = '_';
		printHex(m, this_fn, 0xFF00FF00); //green for call
	}
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit  (void *this_fn, void *call_site){
	if(loaded == 1){
		char m[depth + 1];
		for(int i = 0; i < depth; i++){
			m[i] = ' ';
		}
		m[depth + 0] = 0; //null termination
		m[0] = '_';
		printHex(m, this_fn, 0xFFFF0000); //red for return
		depth--;
	}
}