//
// uspibind.cpp
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <uspios.h>
#include <FreeRTOS.h>
#include <interrupts.h>
#include <task.h>
#include <video.h>
#include <mailbox.h>

//#include <uspienv/timer.h>
//#include <uspienv/interrupt.h>
//#include <uspienv/bcmpropertytags.h>
//#include <uspienv/logger.h>
//#include <uspienv/debug.h>
//#include <uspienv/util.h>
//#include <uspienv/assert.h>

void MsDelay (unsigned nMilliSeconds){
println("msDelay", 0xFFFFFFFF);
	//vTaskDelay(nMilliSeconds);//TimerMsDelay (TimerGet (), nMilliSeconds);
}

void usDelay (unsigned nMicroSeconds){
println("usDelay", 0xFFFFFFFF);
	//vTaskDelay(1);//TimerusDelay (TimerGet (), nMicroSeconds);
}

unsigned StartKernelTimer (unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext){
println("StartKernelTimer", 0xFFFFFFFF);
	return 1;//TimerStartKernelTimer (TimerGet (), nDelay, pHandler, pParam, pContext);
}

void CancelKernelTimer (unsigned hTimer){
	println("CancelKernelTimer", 0xFFFFFFFF);//TimerCancelKernelTimer (TimerGet (), hTimer);
}

void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pfnHandler, void *pParam){
	//InterruptSystemConnectIRQ (InterruptSystemGet (), nIRQ, pHandler, pParam);
	RegisterInterrupt(nIRQ, pfnHandler, pParam);
}

int SetPowerStateOn (unsigned nDeviceId){
	unsigned int mailbuffer[8] __attribute__((aligned (16)));

	//set power state
	mailbuffer[0] = 8 * 4;		//mailbuffer size
	mailbuffer[1] = 0;			//response code
	mailbuffer[2] = 0x00028001;	//set power state
	mailbuffer[3] = 8;			//value buffer size
	mailbuffer[4] = 0;			//Req. + value length (bytes)
	mailbuffer[5] = nDeviceId;	//device id
	mailbuffer[6] = 1;			//state
	mailbuffer[7] = 0;			//terminate buffer

	//spam mail until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);
	}

	return 1;
}

int GetMACAddress (unsigned char Buffer[6]){
	unsigned int mailbuffer[7] __attribute__((aligned (16)));

	//set power state
	mailbuffer[0] = 7 * 4;		//mailbuffer size
	mailbuffer[1] = 0;			//response code
	mailbuffer[2] = 0x00010003;	//get mac
	mailbuffer[3] = 6;			//value buffer size
	mailbuffer[4] = 0;			//Req. + value length (bytes)
	mailbuffer[5] = 0;			//12 34 56 AB CD EF 00 00
	mailbuffer[6] = 0;			//terminate buffer

	//spam mail until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);
	}

	memcpy(Buffer, *(&mailbuffer + 24), 6);
char str[7] = "aaaaaa";
strncpy(str, Buffer, 6);
println(str, 0xFFFFFFFF);
println("did it worked?", 0xFFFFFFFF);

	/*TBcmPropertyTags Tags;
	BcmPropertyTags (&Tags);
	TPropertyTagMACAddress MACAddress;
	if (!BcmPropertyTagsGetTag (&Tags, PROPTAG_GET_MAC_ADDRESS, &MACAddress, sizeof MACAddress, 0))
	{
		_BcmPropertyTags (&Tags);

		return 0;
	}

	memcpy (Buffer, MACAddress.Address, 6);
	
	_BcmPropertyTags (&Tags);*/

	return 1;
}

void LogWrite (const char *pSource, unsigned Severity, const char *pMessage, ...)
{
	/*va_list var;
	va_start (var, pMessage);

	LoggerWriteV (LoggerGet (), pSource, (TLogSeverity) Severity, pMessage, var);

	va_end (var);*/
	println(pMessage, 0xFFFFFFFF);
}

#ifndef NDEBUG

void uspi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine)
{
	println("assert failed", 0xFFFFFFFF);//assertion_failed (pExpr, pFile, nLine);
	//while(1){;} //system failure
}

void DebugHexdump (const void *pBuffer, unsigned nBufLen, const char *pSource)
{
	//debug_hexdump (pBuffer, nBufLen, pSource);
}

#endif

void* malloc(unsigned nSize){
	return pvPortMalloc(nSize);
}

void free(void* pBlock){
	vPortFree(pBlock);
}

void *memset(void *s, int c, size_t n){
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n){
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}

void *memcpy(void *dest, const void *src, size_t n){
    char *dp = dest;
    const char *sp = src;
    while (n--)
        *dp++ = *sp++;
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n){
    char *ret = dest;
    do {
        if (!n--)
            return ret;
    } while (*dest++ = *src++);
    while (n--)
        *dest++ = 0;
    return ret;
}