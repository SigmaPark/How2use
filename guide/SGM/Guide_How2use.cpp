/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Guide_How2use.hpp"


static void intro()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Introduction")
	<<	L"This library helps you make manual documents for a code library "
	<<	L"with its example codes." << sgm::h2u::newl
	<<	L"You can easily create the documents as Markdown format just by writing example codes."
	<<	sgm::h2u::empty_line;
}


static void Math_Expression()
{
	sgm::h2u::mdo 
	<<	sgm::h2u::Title(L"Math Expression")
	<<	L"Markdown supports writing mathematical expressions using LaTeX syntex." 
	<<	sgm::h2u::newl
	<<	sgm::h2u::Load_code_block(L"math_expression_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(math_expression_ex)
	sgm::h2u::mdo << LR"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";
END_CODE_BLOCK(math_expression_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;
}


static void Code_Block()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Code Block")
	<<	L"Macro \"BEGIN_CODE_BLOCK\" and \"END_CODE_BLOCK\" captures codes." << sgm::h2u::newl
	<<	sgm::h2u::newl;

BEGIN_CODE_BLOCK(code_block_example_show)

BEGIN_CODE_BLOCK(code_block_ex)
	int sum = 0;

	for(int i = 1;  i <= 10;  ++i)
		sum += i;

	SGM_H2U_ASSERT(sum == 55);
END_CODE_BLOCK(code_block_ex)

END_CODE_BLOCK(code_block_example_show)

	sgm::h2u::mdo << sgm::h2u::Load_code_block(L"code_block_example_show") << sgm::h2u::newl;

	sgm::h2u::mdo << sgm::h2u::empty_line;
}


static void Assertions()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Helper Functions for Assertion")
	<<	L"You can not only show codes but also assert the result from the code is correct "
	<<	L"using assertion helper functions. They check if the code works fine. When the code "
	<<	L"fails the assertion, log message will be shown and "
	<<	L"Markdown document file won't be exported "
	<<	L"(if there was an old document already, it will be eliminated) . " 
	<<	L"Therefore it is guaranteed the code result is correct "
	<<	L"if the document file is successfully created."
	<<	sgm::h2u::empty_line;


	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"SGM_H2U_ASSERT", 2)
	<<	L"\"SGM_H2U_ASSERT\" checks an boolean expression is true." << sgm::h2u::newl;

BEGIN_CODE_BLOCK(is_true_ex)
	int result = 1 + 1;

	SGM_H2U_ASSERT(result == 2);
END_CODE_BLOCK_AND_LOAD(is_true_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;


	sgm::h2u::mdo 
	<<	sgm::h2u::Title(L"sgm::h2u::Are_All_True / sgm::h2u::Are_N_True", 2)
	<<	L"\"sgm::h2u::Are_All_True\" checks whether all elements in a range on 1st parameter "
	<<	L"satisfy a condition function given as 2nd parameter. " 
	<<	L"\"sgm::h2u::Are_N_True\" do the same thing on partial range from an iterator ." 
	<<	sgm::h2u::newl;

BEGIN_CODE_BLOCK(are_all_true_ex)
	{
		std::initializer_list<int> range{2, 4, 6, 8, 10, 12};

		auto are_even_f = [](int n)-> bool{  return n % 2 == 0;  };
		auto are_less_than_10_f = [](int n)-> bool{  return n < 10;  };

		SGM_H2U_ASSERT
		(	sgm::h2u::Are_All_True(range, are_even_f)
		&&	sgm::h2u::Are_N_True(range.begin(), 4, are_less_than_10_f)
		);
	}
END_CODE_BLOCK_AND_LOAD(are_all_true_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;


	sgm::h2u::mdo 
	<<	sgm::h2u::Title(L"sgm::h2u::Are_All_Equivalent_to / sgm::h2u::Are_N_Equivalent_to", 2)
	<<	L"\"sgm::h2u::Are_All_Equivalent_to\" checks whether all elements in a range on 1st "
	<<	L"parameter are equivalent to 2nd parameter. You can specify the function that determines "
	<<	L"whether they are equivalent on 3rd parameter ( operator== as default ) . "
	<<	L"\"sgm::h2u::Are_N_Equivalent_to\" do the same thing on partial range "
	<<	L"from an iterator . " << sgm::h2u::newl;

BEGIN_CODE_BLOCK(are_all_equivalent_ex)
	{
		std::initializer_list<int> range{7, 7, 7, 7};

		sgm::h2u::Are_All_Equivalent_to(range, 7);
	}
	{
		std::initializer_list<int> 
			range0{7, 7, 7, -7, -7},
			range1{3, 5, 7, 7, 7, -7, -7, -5, -3};

		auto abs_value_are_same_f 
		=	[](int n0, int n1)-> bool
			{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			};

		SGM_H2U_ASSERT
		(	sgm::h2u::Are_All_Equivalent_to(range0, 7, abs_value_are_same_f)
		&&	sgm::h2u::Are_N_Equivalent_to(range0.begin(), 3, 7)
		&&	sgm::h2u::Are_N_Equivalent_to(range1.begin() + 2, 3, 7)
		&&	sgm::h2u::Are_N_Equivalent_to(range1.begin() + 2, 5, 7, abs_value_are_same_f)
		);
	}
END_CODE_BLOCK_AND_LOAD(are_all_equivalent_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;


	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"sgm::h2u::Are_Equivalent_Ranges", 2)
	<<	L"\"sgm::h2u::Are_Equivalent_Ranges\" checks whether two ranges given as 1st and 2nd "
	<<	L"parameters are the same (the number of elements are same and have the same elements "
	<<	L"in the same order) . You can specify the function that determines whether the elements "
	<<	L"are same each others ( operator== as default ) . " << sgm::h2u::newl;

BEGIN_CODE_BLOCK(are_equivalent_ranges_ex)
	{
		std::initializer_list<int> 
			range0{2, 4, 6, 8}, 
			range1{2, 4, 6, 8},
			range2{2, -4, 6, -8};

		auto abs_value_are_same_f 
		=	[](int n0, int n1)-> bool
			{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			};	

		SGM_H2U_ASSERT
		(	sgm::h2u::Are_Equivalent_Ranges(range0, range1)
		&&	sgm::h2u::Are_Equivalent_Ranges(range1, range2, abs_value_are_same_f)
		);
	}
END_CODE_BLOCK_AND_LOAD(are_equivalent_ranges_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;	
}


static void External_Resources()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"External Resources")
	<<	L"All external materials you want to attach to a document are to be located at "
	<<	L"\"\\md_materials\"" << sgm::h2u::empty_line;

	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Text File", 2)
	<<	L"Use a macro function \"LOAD_DESCRIPTION_FILE\"" << sgm::h2u::newl
	<<	sgm::h2u::Load_code_block(L"descrip_file_ex") << sgm::h2u::newl;


	{
		sgm::h2u::md_block_guard mbg;

BEGIN_CODE_BLOCK(descrip_file_ex)
		sgm::h2u::mdo << sgm::h2u::Load_description_file(L"YOLO.txt");
END_CODE_BLOCK(descrip_file_ex)
	}

	sgm::h2u::mdo << sgm::h2u::empty_line;
	
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Image File", 2)
	<<	L"Use \"sgm::h2u::Load_image\" function." << sgm::h2u::newl
	<<	sgm::h2u::Load_code_block(L"img_ex") << sgm::h2u::newl;


BEGIN_CODE_BLOCK(img_ex)
	{
		sgm::h2u::html_block_guard hbg(L"center");

		sgm::h2u::mdo << sgm::h2u::Load_image(L"sample_image.jpg", 720);
	}
END_CODE_BLOCK(img_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;

	sgm::h2u::mdo
	<<	L"The 2nd parameter means image width to show " 
	<<	L"and can be omitted(then it shows the image by original size)."
	<<	sgm::h2u::empty_line;
}


static void Guards()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Guards")
	<<	sgm::h2u::Title(L"Block Guard", 2)
	<<	sgm::h2u::Load_code_block(L"md_guard_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(md_guard_ex)
	{
		sgm::h2u::md_block_guard mbg;

		sgm::h2u::mdo 
		<<	L"Until \"mbg\" is alive(before its destructor is excuted), "
		<<	L"all contents are in box tab." << sgm::h2u::newl;
	}
END_CODE_BLOCK(md_guard_ex)


	sgm::h2u::mdo
	<<	sgm::h2u::newl
	<<	sgm::h2u::Title(L"HTML Guard", 2)
	<<	sgm::h2u::Load_code_block(L"html_guard_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(html_guard_ex)
	{
		sgm::h2u::html_block_guard hbg(L"center strong blockquote");
		
		sgm::h2u::mdo 
		<<	L"Until \"hbg\" is alive, "
		<<	L"all contents are aligned at center, emphasized and quoted in block." 
		<<	sgm::h2u::newl;
	}
END_CODE_BLOCK(html_guard_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;
	
	sgm::h2u::mdo 
	<<	L"Another way to apply HTML tags is to use \"sgm::h2u::HTML_tag\" function."
	<<	sgm::h2u::newl
	<<	sgm::h2u::Load_code_block(L"html_tag_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(html_tag_ex)
	sgm::h2u::mdo 
	<<	sgm::h2u::HTML_tag(L"HTML tags to a short messages.", L"center strong blockquote");
END_CODE_BLOCK(html_tag_ex)

	
	sgm::h2u::mdo << sgm::h2u::empty_line;
}


static void Literal_Suffixes()
{
	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"Literal Suffixes")
	<<	sgm::h2u::Title(L"\"\"_mdo for Tabless Description", 2)
	<<	sgm::h2u::Load_code_block(L"mdo_suffix_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(mdo_suffix_ex)
	sgm::h2u::mdo
	<<	LR"(
			If you want to write something verbose, 
			pre-defined literal operator ""_mdo should be helpful.

			As if you write a normal script or comment, 
			describe what you want to explain freely.
					All blank spaces and tabs on left will be removed.
		)"_mdo;
END_CODE_BLOCK(mdo_suffix_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;


	sgm::h2u::mdo
	<<	sgm::h2u::Title(L"\"\"_code for Pseudo Code", 2)
	<<	L"Sometimes, it can be effective to use peudo code which is not excutable "
	<<	L"but easy to understand when you explain a logic and how it works. "
	<<	L"A pre-defined literal operator \"\"_code helps you write the pseudo code very easily."
	<<	sgm::h2u::newl
	<<	sgm::h2u::Load_code_block(L"pseudo_code_ex") << sgm::h2u::newl;

BEGIN_CODE_BLOCK(pseudo_code_ex)
	sgm::h2u::mdo 
	<<	LR"(
			Selection_Sort(A[], n)
			    for last <- downto 2
			        Find the greatest element A[k] among A[1...last]
			        Swap A[k] and A[last]
		)"_code;
END_CODE_BLOCK(pseudo_code_ex)

	sgm::h2u::mdo << sgm::h2u::empty_line;
}


static void outro()
{
	sgm::h2u::mdo 
	<<	L"----" << sgm::h2u::newl
	<<	sgm::h2u::HTML_tag(L"Thank you for watching", L"strong center") << sgm::h2u::newl;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_HOW2USE_TESTS(sgm::h2u::Guide_, How2use, /**/)
{	::intro
,	::Math_Expression
,	::Code_Block
,	::Assertions
,	::External_Resources
,	::Guards
,	::Literal_Suffixes
,	::outro
};