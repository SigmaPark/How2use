/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#

#include "Guide_How2use.hpp"
#include <deque>
#include <algorithm>
#include <memory>

static void intro(){
	h2u::mdo
	<< h2u::Title(L"Introduction")
	<< L"This library helps you make manual documents for a code library "
	<< L"with its example codes." << h2u::newl
	<< L"You can easily create the documents as Markdown format just by writing example codes."
	<< h2u::empty_line;
}

static void Math_Expression(){
	h2u::mdo
	<< h2u::Title(L"Math Expression")
	<< L"Markdown supports writing mathematical expressions using LaTeX syntex." << h2u::newl
	<< h2u::Load_code_block(L"math_expression_ex") << h2u::newl;

BEGIN_CODE_BLOCK(math_expression_ex)
	h2u::mdo << LR"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";
END_CODE_BLOCK(math_expression_ex)

	h2u::mdo << h2u::empty_line;
}

static void Code_Block(){
	h2u::mdo
	<< h2u::Title(L"Code Block")
	<< L"Macro \"BEGIN_CODE_BLOCK\" and \"END_CODE_BLOCK\" captures codes." << h2u::newl
	<< h2u::newl;

BEGIN_CODE_BLOCK(code_block_example_show)

BEGIN_CODE_BLOCK(code_block_ex)
	int sum = 0;

	for(int i = 1; i <= 10; ++i){
		sum += i;
	}

	H2U_ASSERT(sum == 55);
END_CODE_BLOCK(code_block_ex)

END_CODE_BLOCK(code_block_example_show)

	h2u::mdo << h2u::Load_code_block(L"code_block_example_show") << h2u::newl;
	h2u::mdo << h2u::empty_line;
}

static void Assertions(){
	h2u::mdo
	<< h2u::Title(L"Helper Functions for Assertion")
	<< L"You can not only show codes but also assert the result from the code is correct "
	<< L"using assertion helper functions. They check if the code works fine. When the code "
	<< L"fails the assertion, log message will be shown and "
	<< L"Markdown document file won't be exported "
	<< L"(if there was an old document already, it will be eliminated) . "
	<< L"Therefore it is guaranteed the code result is correct "
	<< L"if the document file is successfully created."
	<< h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"H2U_ASSERT", 2)
	<< L"\"H2U_ASSERT\" checks an boolean expression is true." << h2u::newl;

BEGIN_CODE_BLOCK(is_true_ex)
	int result = 1 + 1;

	H2U_ASSERT(result == 2);
END_CODE_BLOCK_AND_LOAD(is_true_ex)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"h2u::Are_All_True / h2u::Are_N_True", 2)
	<< L"\"h2u::Are_All_True\" checks whether all elements in a range on 1st parameter "
	<< L"satisfy a condition function given as 2nd parameter. "
	<< L"\"h2u::Are_N_True\" do the same thing on partial range from an iterator ."
	<< h2u::newl;

	{
	BEGIN_CODE_BLOCK(are_all_true_ex)
		std::initializer_list<int> range{ 2, 4, 6, 8, 10, 12 };

		auto are_even_f = [](int n)->bool{ return n % 2 == 0; };
		auto are_less_than_10_f = [](int n)->bool{ return n < 10; };

		H2U_ASSERT(
			h2u::Are_All_True(range, are_even_f)
			&& h2u::Are_N_True(range.begin(), 4, are_less_than_10_f)
		);
	END_CODE_BLOCK_AND_LOAD(are_all_true_ex)
	}

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"h2u::Are_All_Equivalent_to / h2u::Are_N_Equivalent_to", 2)
	<< L"\"h2u::Are_All_Equivalent_to\" checks whether all elements in a range on 1st "
	<< L"parameter are equivalent to 2nd parameter. You can specify the function that determines "
	<< L"whether they are equivalent on 3rd parameter ( operator== as default ) . "
	<< L"\"h2u::Are_N_Equivalent_to\" do the same thing on partial range "
	<< L"from an iterator . "
	<< h2u::newl;

BEGIN_CODE_BLOCK(are_all_equivalent_ex)
	{
		std::initializer_list<int> range{ 7, 7, 7, 7 };

		h2u::Are_All_Equivalent_to(range, 7);
	}
	{
		std::initializer_list<int>
			range0{ 7, 7, 7, -7, -7 },
			range1{ 3, 5, 7, 7, 7, -7, -7, -5, -3 }
		;

		auto
			abs_value_are_same_f
			= [](int n0, int n1)->bool{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			}
		;

		H2U_ASSERT(
			h2u::Are_All_Equivalent_to(range0, 7, abs_value_are_same_f)
			&& h2u::Are_N_Equivalent_to(range0.begin(), 3, 7)
			&& h2u::Are_N_Equivalent_to(range1.begin() + 2, 3, 7)
			&& h2u::Are_N_Equivalent_to(range1.begin() + 2, 5, 7, abs_value_are_same_f)
		);
	}
END_CODE_BLOCK_AND_LOAD(are_all_equivalent_ex)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"h2u::Are_Equivalent_Ranges", 2)
	<< L"\"h2u::Are_Equivalent_Ranges\" checks whether two ranges given as 1st and 2nd "
	<< L"parameters are the same (the number of elements are same and have the same elements "
	<< L"in the same order) . You can specify the function that determines whether the elements "
	<< L"are same each others ( operator== as default ) . "
	<< h2u::newl;

	{
	BEGIN_CODE_BLOCK(are_equivalent_ranges_ex)
		std::initializer_list<int>
			range0{ 2, 4, 6, 8 },
			range1{ 2, 4, 6, 8 },
			range2{ 2, -4, 6, -8 }
		;

		auto
			abs_value_are_same_f
			= [](int n0, int n1)->bool{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			}
		;

		H2U_ASSERT(
			h2u::Are_Equivalent_Ranges(range0, range1)
			&& h2u::Are_Equivalent_Ranges(range1, range2, abs_value_are_same_f)
		);
	END_CODE_BLOCK_AND_LOAD(are_equivalent_ranges_ex)
	}

	h2u::mdo << h2u::empty_line;
}

static void Specimen_and_State(){
	h2u::mdo
	<< h2u::Title(L"h2u::Specimen")
	<< L"The library provides a powerful behavior verification tool, \"h2u::Specimen\". "
	<< h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"h2u::Specimen::State", 2)
	<< L"It tracks object lifecycle events through a State flag, allowing you to verify "
	<< L"how objects are constructed, copied, moved, and destroyed in your code."
	<< h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"State Flag for Lifecycle Tracking", 3)
	<< L"The Specimen class maintains a State enum that records the most recent lifecycle event. "
	<< L"You can directly check this State to verify object behavior."
	<< h2u::newl;

	{
	BEGIN_CODE_BLOCK(specimen_state_basic_ex)
		h2u::Specimen s1;
		H2U_ASSERT(s1.state() == h2u::Specimen::State::DEFAULT_CONSTRUCTION);

		h2u::Specimen s2(42);
		H2U_ASSERT(
			s2.state() == h2u::Specimen::State::MANUAL_CONSTRUCTION
			&& s2.value() == 42
		);

		h2u::Specimen s3 = s2;
		H2U_ASSERT(
			s3.state() == h2u::Specimen::State::COPY_CONSTRUCTION
			&& s3.value() == 42
		);
	END_CODE_BLOCK_AND_LOAD(specimen_state_basic_ex)
	}

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"Move Semantics Verification", 3)
	<< L"When an object is moved, the source object's State becomes MOVE_AWAY, "
	<< L"while the destination object's State indicates the move operation type."
	<< h2u::newl;

	{
	BEGIN_CODE_BLOCK(specimen_move_ex)
		h2u::Specimen s1(100);

		h2u::Specimen s2 = std::move(s1);

		H2U_ASSERT(
			s2.state() == h2u::Specimen::State::MOVE_CONSTRUCTION
			&& s2.value() == 100
			&& s1.state() == h2u::Specimen::State::MOVE_AWAY
		);

		h2u::Specimen s3(200);
		s3 = std::move(s2);

		H2U_ASSERT(
			s3.state() == h2u::Specimen::State::MOVE_ASSIGNMENT
			&& s3.value() == 100
			&& s2.state() == h2u::Specimen::State::MOVE_AWAY
		);
	END_CODE_BLOCK_AND_LOAD(specimen_move_ex)
	}

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< L"The State flag provides immediate, direct verification of an object's lifecycle status. "
	<< L"This is particularly useful when testing how your code handles object semantics, "
	<< L"ensuring that operations like copy elision or move optimization occur as expected."
	<< h2u::empty_line;
}

static void Specimen_Log_Tracking(){
	h2u::mdo
	<< h2u::Title(L"Testing with Specimen Logger", 2)
	<< L"For complex scenarios where you need to verify a sequence of lifecycle events, "
	<< L"Specimen provides a logger system. By implementing a custom logger, you can "
	<< L"record and analyze the entire history of object operations."
	<< h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"Recording Logger Implementation", 3)
	<< L"Create a custom logger by inheriting from \"h2u::Specimen_Logger\". "
	<< L"This example uses a deque to record all lifecycle events."
	<< h2u::newl;

BEGIN_CODE_BLOCK(recording_logger_impl)
	class Recording_Specimen_Logger
	:
		public h2u::Specimen_Logger
	{
	private:
		std::deque<std::wstring> _record;
	public:
		std::deque<std::wstring> const &record;

		Recording_Specimen_Logger() noexcept
		:
			h2u::Specimen_Logger(),
			_record(),
			record(_record)
		{}

		void log(std::wstring const &msg) override{ _record.emplace_back(msg); }
		void reset() noexcept{ _record.clear(); }

		~Recording_Specimen_Logger() override = default;
	};
END_CODE_BLOCK_AND_LOAD(recording_logger_impl)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"Testing Template Behavior", 3)
	<< L"The logger system is especially powerful for verifying how template classes "
	<< L"handle objects. Use \"h2u::Specimen_Log_Guard\" to enable logging within "
	<< L"a scope, then examine the recorded events."
	<< h2u::newl;

BEGIN_CODE_BLOCK(specimen_template_test)
	Recording_Specimen_Logger spec_log;

	{
		h2u::Specimen_Log_Guard const monitor_guard(spec_log);

		h2u::Specimen s1;
		h2u::Specimen s2 = s1;
	}

	//	Verify copy constructor was called during monitoring.
	H2U_ASSERT(
		std::find(spec_log.record.begin(), spec_log.record.end(), L"copy_construction")
		!= spec_log.record.end()
	);

	spec_log.reset();

	{
		h2u::Specimen_Log_Guard const monitor_guard(spec_log);

		auto sp_s1 = std::make_shared<h2u::Specimen>();
		std::shared_ptr<h2u::Specimen> sp_s2 = sp_s1;
	}

	//	Verify std::shared_ptr<T> doesn't call T's copy constructor.
	H2U_ASSERT(
		std::find(spec_log.record.begin(), spec_log.record.end(), L"copy_construction")
		== spec_log.record.end()
	);
END_CODE_BLOCK_AND_LOAD(specimen_template_test)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< L"This testing approach is invaluable when implementing custom template classes "
	<< L"(like decorators, wrappers, or containers) where you need to ensure optimal "
	<< L"forwarding behavior and avoid unnecessary copies. " << h2u::newl
	<< L"By recording event sequences, you can verify not just what the final state is, "
	<< L"but also how the program arrived at that state, revealing hidden copies or "
	<< L"suboptimal forwarding that might otherwise go unnoticed."
	<< h2u::empty_line;
}

static void External_Resources(){
	h2u::mdo
	<< h2u::Title(L"External Resources")
	<< L"All external materials you want to attach to a document are to be located at "
	<< L"\"/md_materials\""
	<< h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"Text File", 2)
	<< L"Use a macro function \"LOAD_DESCRIPTION_FILE\"" << h2u::newl
	<< h2u::Load_code_block(L"descrip_file_ex")
	<< h2u::newl;

	{
		h2u::md_block_guard mbg;

	BEGIN_CODE_BLOCK(descrip_file_ex)
		h2u::mdo << h2u::Load_description_file(L"YOLO.txt");
	END_CODE_BLOCK(descrip_file_ex)
	}

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"Image File", 2)
	<< L"Use \"h2u::Load_image\" function." << h2u::newl
	<< h2u::Load_code_block(L"img_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(img_ex)
	{
		h2u::html_block_guard hbg(L"center");

		h2u::mdo << h2u::Load_image(L"sample_image.jpg", 720);
	}
END_CODE_BLOCK(img_ex)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< L"The 2nd parameter means image width to show "
	<< L"and can be omitted(then it shows the image by original size)."
	<< h2u::empty_line;
}

static void Guards(){
	h2u::mdo
	<< h2u::Title(L"Guards")
	<< h2u::Title(L"Block Guard", 2)
	<< h2u::Load_code_block(L"md_guard_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(md_guard_ex)
	{
		h2u::md_block_guard mbg;

		h2u::mdo
		<< L"Until \"mbg\" is alive(before its destructor is excuted), "
		<< L"all contents are in box tab."
		<< h2u::newl;
	}
END_CODE_BLOCK(md_guard_ex)

	h2u::mdo
	<< h2u::newl << h2u::Title(L"HTML Guard", 2)
	<< h2u::Load_code_block(L"html_guard_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(html_guard_ex)
	{
		h2u::html_block_guard hbg(L"center strong blockquote");

		h2u::mdo
		<< L"Until \"hbg\" is alive, "
		<< L"all contents are aligned at center, emphasized and quoted in block."
		<< h2u::newl;
	}
END_CODE_BLOCK(html_guard_ex)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< L"Another way to apply HTML tags is to use \"h2u::HTML_tag\" function." << h2u::newl
	<< h2u::Load_code_block(L"html_tag_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(html_tag_ex)
	h2u::mdo << h2u::HTML_tag(L"HTML tags to a short messages.", L"center strong blockquote");
END_CODE_BLOCK(html_tag_ex)

	h2u::mdo << h2u::empty_line;
}

static void Literal_Suffixes(){
	h2u::mdo
	<< h2u::Title(L"Literal Suffixes")
	<< h2u::Title(L"\"\"_mdo for Tabless Description", 2)
	<< h2u::Load_code_block(L"mdo_suffix_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(mdo_suffix_ex)
	h2u::mdo
	<< LR"(
		If you want to write something verbose,
		pre-defined literal operator ""_mdo should be helpful.

		As if you write a normal script or comment,
		describe what you want to explain freely.
				All blank spaces and tabs on left will be removed.
	)"_mdo;
END_CODE_BLOCK(mdo_suffix_ex)

	h2u::mdo << h2u::empty_line;

	h2u::mdo
	<< h2u::Title(L"\"\"_code for Pseudo Code", 2)
	<< L"Sometimes, it can be effective to use peudo code which is not excutable "
	<< L"but easy to understand when you explain a logic and how it works. "
	<< L"A pre-defined literal operator \"\"_code helps you write the pseudo code very easily."
	<< h2u::newl << h2u::Load_code_block(L"pseudo_code_ex")
	<< h2u::newl;

BEGIN_CODE_BLOCK(pseudo_code_ex)
	h2u::mdo
	<< LR"(
		Selection_Sort(A[], n)
		    for last <- downto 2
		        Find the greatest element A[k] among A[1...last]
		        Swap A[k] and A[last]
	)"_code;
END_CODE_BLOCK(pseudo_code_ex)

	h2u::mdo << h2u::empty_line;
}

static void Outro(){
	h2u::mdo
	<< L"----" << h2u::newl
	<< h2u::HTML_tag(L"Thank you for watching", L"strong center")
	<< h2u::newl;
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

H2U_HOW2USE_TESTS(h2u::Guide_, How2use, /**/){
	::intro,
	::Math_Expression,
	::Code_Block,
	::Assertions,
	::Specimen_and_State,
	::Specimen_Log_Tracking,
	::External_Resources,
	::Guards,
	::Literal_Suffixes,
	::Outro
};
