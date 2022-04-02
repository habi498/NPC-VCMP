#include "ConsoleUtils.h"

void OutputMessage( const char * msg )
{
	#ifdef WIN32
		HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );

		CONSOLE_SCREEN_BUFFER_INFO csbBefore;
		GetConsoleScreenBufferInfo( hstdout, &csbBefore );
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN );
		std::cout << "[MODULE]  ";
		
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY );
		std::cout << msg << std::endl;

		SetConsoleTextAttribute( hstdout, csbBefore.wAttributes );
	#else
		printf( "%c[0;32m[MODULE]%c[0;37m %s\n", 27, 27, msg );
	#endif
}

void OutputWarning( const char * msg )
{
	#ifdef WIN32
		HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );

		CONSOLE_SCREEN_BUFFER_INFO csbBefore;
		GetConsoleScreenBufferInfo( hstdout, &csbBefore );
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
		std::cout << "[WARNING] ";
		
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY );
		std::cout << msg << std::endl;

		SetConsoleTextAttribute( hstdout, csbBefore.wAttributes );
	#else
		printf( "%c[0;30m[WARNING]%c[0;37m %s\n", 27, 27, msg );
	#endif
}

void OutputError( const char * msg )
{
	#ifdef WIN32
		HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );

		CONSOLE_SCREEN_BUFFER_INFO csbBefore;
		GetConsoleScreenBufferInfo( hstdout, &csbBefore );
		SetConsoleTextAttribute( hstdout, FOREGROUND_RED | FOREGROUND_INTENSITY );
		std::cout << "[ERROR]   ";
		
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY );
		std::cout << msg << std::endl;

		SetConsoleTextAttribute( hstdout, csbBefore.wAttributes );
	#else
		printf( "%c[0;30m[ERROR]%c[0;37m %s\n", 27, 27, msg );
	#endif
}