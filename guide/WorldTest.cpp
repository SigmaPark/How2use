/*  SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#

#include "WorldTest.hpp"
#include <iostream>

static void Test() noexcept(false);

template<class = void>
static void Log_message() noexcept{}

template<class A, class ...ARGS>
static void Log_message(A &&a, ARGS &&...args) noexcept{
	std::wcout << static_cast<A &&>(a);

	Log_message( static_cast<ARGS &&>(args)... );
}

static void System_pause() noexcept{
	std::wcout << L"Press Enter key to continue. . .";
	std::cin.get();
}

auto wt::Tests(wchar_t const * const module_title) noexcept->bool{
	::Log_message(
		L"//========//========//========//========//=======#\n",
		module_title, L" test Start\n"
	);

	try{
		::Test();

		::Log_message(
			module_title, L" test Complete\n",
			L"//========//========//========//========//=======#\n"
		);

		::System_pause();

		return true;
	} catch(...){
		std::wcout << L"Error occurs!\n";
	}

	::Log_message(
		module_title, L" test Failed",
		L"//========//========//========//========//=======#\n"
	);

	::System_pause();

	return false;
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

#include "Guide_How2use.hpp"

void Test() noexcept(false){
	h2u::Guide_How2use::test();
}

auto main(int const, char const * const[])->int{
	wchar_t const * const
		os
#if defined(_WINDOWS_SOLUTION_)
		= L"Windows"
#elif defined(_POSIX_SOLUTION_)
		= L"POSIX"
#else
		= L"Others"
#endif
	;

	return wt::Tests(os), 0;
}
