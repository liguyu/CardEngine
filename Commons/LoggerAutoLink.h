/*
  log4cplus 1.0.4程序库的自动连接模块
*/

#ifndef __LOGGER_AUTO_LINK_INC
#define __LOGGER_AUTO_LINK_INC

#ifdef _MSC_VER
	#if defined( _MT ) && defined( _DLL )  // dynamic vc runtime lib
    #ifdef _UNICODE
			#if defined( _DEBUG )
				#pragma comment(lib, "log4cplusUD.lib")
			#else
				#pragma comment(lib, "log4cplusU.lib")
			#endif
		#else
			#if defined( _DEBUG )
				#pragma comment(lib, "log4cplusD.lib")
			#else
				#pragma comment(lib, "log4cplus.lib")
			#endif
		#endif
	#else  // static vc runtime lib
		#ifdef _UNICODE
			#if defined( _DEBUG )
				#pragma comment(lib, "log4cplusUSD.lib")
			#else
				#pragma comment(lib, "log4cplusUS.lib")
			#endif
		#else
			#if defined( _DEBUG )
				#pragma comment(lib, "log4cplusSD.lib")
			#else
				#pragma comment(lib, "log4cplusS.lib")
			#endif
		#endif
  #endif
#endif

#endif  // __LOGGER_AUTO_LINK_INC
