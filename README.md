# Introduction
This library helps you make manual documents for a code library with its example codes.  
You can easily create the documents as Markdown format just by writing example codes.

&nbsp;  

# Math Expression
Markdown supports writing mathematical expressions using LaTeX syntex.  
```cpp
sgm::h2u::mdo << LR"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";  
```
  
$$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$

&nbsp;  

# Code Block
Macro "BEGIN_CODE_BLOCK" and "END_CODE_BLOCK" captures codes.  
  
```cpp
BEGIN_CODE_BLOCK(code_block_ex)  
	int sum = 0;  
  
	for(int i = 1;  i <= 10;  ++i)  
		sum += i;  
  
	SGM_H2U_ASSERT(sum == 55);  
END_CODE_BLOCK(code_block_ex)  
  
```
  


&nbsp;  

# Helper Functions for Assertion
You can not only show codes but also assert the result from the code is correct using assertion helper functions. They check if the code works fine. When the code fails the assertion, log message will be shown and Markdown document file won't be exported (if there was an old document already, it will be eliminated) . Therefore it is guaranteed the code result is correct if the document file is successfully created.

&nbsp;  

## SGM_H2U_ASSERT
"SGM_H2U_ASSERT" checks an boolean expression is true.  
```cpp
int result = 1 + 1;  
  
SGM_H2U_ASSERT(result == 2);  
```


&nbsp;  

## sgm::h2u::Are_All_True / sgm::h2u::Are_N_True
"sgm::h2u::Are_All_True" checks whether all elements in a range on 1st parameter satisfy a condition function given as 2nd parameter. "sgm::h2u::Are_N_True" do the same thing on partial range from an iterator .  
```cpp
{  
	std::initializer_list<int> range{2, 4, 6, 8, 10, 12};  
  
	auto are_even_f = [](int n)-> bool{  return n % 2 == 0;  };  
	auto are_less_than_10_f = [](int n)-> bool{  return n < 10;  };  
  
	SGM_H2U_ASSERT  
	(	sgm::h2u::Are_All_True(range, are_even_f)  
	&&	sgm::h2u::Are_N_True(range.begin(), 4, are_less_than_10_f)  
	);  
}  
```


&nbsp;  

## sgm::h2u::Are_All_Equivalent_to / sgm::h2u::Are_N_Equivalent_to
"sgm::h2u::Are_All_Equivalent_to" checks whether all elements in a range on 1st parameter are equivalent to 2nd parameter. You can specify the function that determines whether they are equivalent on 3rd parameter ( operator== as default ) . "sgm::h2u::Are_N_Equivalent_to" do the same thing on partial range from an iterator .   
```cpp
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
```


&nbsp;  

## sgm::h2u::Are_Equivalent_Ranges
"sgm::h2u::Are_Equivalent_Ranges" checks whether two ranges given as 1st and 2nd parameters are the same (the number of elements are same and have the same elements in the same order) . You can specify the function that determines whether the elements are same each others ( operator== as default ) .   
```cpp
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
```


# Guards
## Block Guard
```cpp
{  
	sgm::h2u::md_block_guard mbg;  
  
	sgm::h2u::mdo   
	<<	L"Until \"mbg\" is alive(before its destructor is excuted), "  
	<<	L"all contents are in box tab." << sgm::h2u::newl;  
}  
```
  
```
Until "mbg" is alive(before its destructor is excuted), all contents are in box tab.  
```
  
## HTML Guard
```cpp
{  
	sgm::h2u::html_block_guard hbg(L"center strong blockquote");  
		  
	sgm::h2u::mdo   
	<<	L"Until \"hbg\" is alive, "  
	<<	L"all contents are aligned at center, emphasized and quoted in block."   
	<<	sgm::h2u::newl;  
}  
```
  
<center><strong><blockquote>Until "hbg" is alive, all contents are aligned at center, emphasized and quoted in block.  
</center></strong></blockquote>

&nbsp;  

Another way to apply HTML tags is to use "sgm::h2u::HTML_tag" function.  
```cpp
sgm::h2u::mdo   
<<	sgm::h2u::HTML_tag(L"HTML tags to a short messages.", L"center strong blockquote");  
```
  
<blockquote><strong><center>HTML tags to a short messages.</center></strong></blockquote>

&nbsp;  

# Literal Suffixes
## ""_mdo for Tabless Description
```cpp
sgm::h2u::mdo  
<<	LR"(  
		If you want to write something verbose,   
		pre-defined literal operator ""_mdo should be helpful.  
  
		As if you write a normal script or comment,   
		describe what you want to explain freely.  
				All blank spaces and tabs on left will be removed.  
	)"_mdo;  
```
  
If you want to write something verbose,   
pre-defined literal operator ""_mdo should be helpful.  
  
As if you write a normal script or comment,   
describe what you want to explain freely.  
All blank spaces and tabs on left will be removed.  
		  


&nbsp;  

## ""_code for Pseudo Code
Sometimes, it can be effective to use peudo code which is not excutable but easy to understand when you explain a logic and how it works. A pre-defined literal operator ""_code helps you write the pseudo code very easily.  
```cpp
sgm::h2u::mdo   
<<	LR"(  
		Selection_Sort(A[], n)  
			for last <- downto 2  
				Find the greatest element A[k] among A[1...last]  
				Swap A[k] and A[last]  
	)"_code;  
```
  
```
Selection_Sort(A[], n)  
	for last <- downto 2  
		Find the greatest element A[k] among A[1...last]  
		Swap A[k] and A[last]  
```


&nbsp;  



## Run Guide Project and Check the More
Build a guide project with script 

```
$ mkdir build
$ cd ./build
$ cmake ..
$ cmake --build .
```

and run the executable.
```
//========//========//========//========//=======#
POSIX test Start
How2use test starts.
How2use test ends.
All cases pass!
//--------//--------//--------//--------//--------
POSIX test Complete
//========//========//========//========//=======#
Press Enter key to continue. . .
```
 You can find "guide/SGM/[doc]\_Guide_How2use.md" files and see more powerful functions!

----  
<center><strong>Thank you for watching</strong></center>  
