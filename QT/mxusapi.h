/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

/*
    Mxusapi.h

	This program offers following subroutines to access UPort parameters.

	BOOL MxSetUARTFIFO(int ComNo, int bFifo)
	BOOL MxSetTxMode(int ComNo, int txmode)
	BOOL MxSetFastFlush(int ComNo, int fastflush)
	BOOL MxSetInterface(int ComNo, int Interface)
	int MxGetUARTFIFO(int ComNo)
	int MxGetTxMode(int ComNo)
	int MxGetFastFlush(int ComNo)
	int MxGetInterface(int ComNo)

	Compliant Products:
	  UPort 1250, 1250I, 1410, 1450, 1450I, 1610-8, 1610-16, 1650-8, 1650-16

	2009-01-08	Jason Chen
*/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MXUSAPI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MXUSAPI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MXUSAPI_EXPORTS
#define MXUSAPI_API __declspec(dllexport)
#else
#define MXUSAPI_API __declspec(dllimport)
#endif

// This class is exported from the mxusapi.dll
class MXUSAPI_API Cmxusapi {
public:
	Cmxusapi(void);
	// TODO: add your methods here.
};

extern MXUSAPI_API int nmxusapi;

MXUSAPI_API int fnmxusapi(void);



#if !defined(MXUAPI_H__EA87D282_66A0_4ab8_B51E_511B9AAE2F07__INCLUDED_)
#define MXUAPI_H__EA87D282_66A0_4ab8_B51E_511B9AAE2F07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#define 	MAXPORTS			256
#define		MXU_RS232			0
#define		MXU_RS422			1
#define		MXU_RS485_2W		2
#define		MXU_RS485_4W		3

#define BOOL bool

extern "C" MXUSAPI_API BOOL MxSetInterface(int ComNo, int Interface);

extern "C" MXUSAPI_API int MxGetInterface(int ComNo);

extern "C" MXUSAPI_API BOOL MxSetUARTFIFO(int ComNo, int bFifo);

extern "C" MXUSAPI_API BOOL MxSetTxMode(int ComNo, int txmode);

extern "C" MXUSAPI_API BOOL MxSetFastFlush(int ComNo, int fastflush);

extern "C" MXUSAPI_API int MxGetUARTFIFO(int ComNo);

extern "C" MXUSAPI_API int MxGetTxMode(int ComNo);

extern "C" MXUSAPI_API int MxGetFastFlush(int ComNo);

//int MxGetUPortRegistry(int ComNo, LPTSTR strRegistry );

int MxGetUPortPID(int ComNo, int * pPortNo);

//HANDLE MxGetUPortHandle(int ComNo, int * pPortNo );

int GetComInfoFromFriendlyName(char * pSource, int * pPortNo);

//BOOL MxSetUPortRegistry(int ComNo, LPTSTR strRegistry, char * RegBuf, int RegSize );

int MxIsFixComFuncEnable( int pPID );


//extern LPCSTR Ldir_FixComReg;

#endif
