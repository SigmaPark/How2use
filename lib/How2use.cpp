/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#

#include "How2use.hpp"
#include <cwchar>
#include <fstream>
#include <queue>
#include <stdexcept>

using std::size_t;
using std::wstring;
using dir_t = wstring;

auto operator ""_mdo(wchar_t const *str, size_t)->h2u::_tabless_description{
	return wstring(str);
}

auto operator ""_code(wchar_t const *str, size_t)->h2u::_code_description{
	return wstring(str);
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

auto h2u::_Mbs_to_Wcs(std::string const &mbs)->std::wstring{
	std::mbstate_t state = {};
	char const *src = mbs.c_str();
	size_t const len = std::mbsrtowcs(nullptr, &src, 0, &state);

	if( len == static_cast<size_t>(-1) ){
		throw std::runtime_error("invalid multibyte sequence in _Mbs_to_Wcs.");
	}

	std::wstring res(len, L'\0');

	if(len != 0){
		src = mbs.c_str();
		state = std::mbstate_t{};
		std::mbsrtowcs(&res.front(), &src, len, &state);
	}

	return res;
}

auto h2u::_Wcs_to_Mbs(std::wstring const &wcs)->std::string{
	std::mbstate_t state = {};
	wchar_t const *src = wcs.c_str();
	size_t const len = std::wcsrtombs(nullptr, &src, 0, &state);

	if( len == static_cast<size_t>(-1) ){
		throw std::runtime_error("invalid wide character in _Wcs_to_Mbs.");
	}

	std::string res(len, '\0');

	if(len != 0){
		src = wcs.c_str();
		state = std::mbstate_t{};
		std::wcsrtombs(&res.front(), &src, len, &state);
	}

	return res;
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

struct h2u::_MD_Stream::_Contents{ std::queue<wstring> q = {}; };

h2u::_MD_Stream::_MD_Stream()
:
	_working_filepath(),
	_md_filepath(),
	_md_materials_dir(),
	_pcnts(new _Contents())
{}

h2u::_MD_Stream::~_MD_Stream(){ delete _pcnts; }

auto h2u::_MD_Stream::instance()->_MD_Stream &{
	static _MD_Stream res;

	return res;
}

void h2u::_MD_Stream::open(dir_t const working_filepath){
	if(is_open()){ return; }

	_working_filepath = working_filepath;

	_md_materials_dir
	= [](dir_t str){
		auto const last_slash = str.find_last_of(L'/');
		str.erase(str.begin() + last_slash, str.end());

		return str + L"/md_materials";
	}(working_filepath);

	_md_filepath
	= [](dir_t str){
		auto const last_dot = str.find_last_of(L'.');
		str.erase(str.begin() + last_dot, str.end());

		auto const last_slash = str.find_last_of(L'/');
		dir_t const direc(str.begin(), str.begin() + last_slash + 1);
		wstring const name(str.begin() + last_slash + 1, str.end());

		return direc + L"[doc]_" + name + L".md";
	}(working_filepath);
}

bool h2u::_MD_Stream::is_open() const{ return _md_filepath != dir_t(); }

void h2u::_MD_Stream::close(){
	_working_filepath = _md_filepath = _md_materials_dir = {};

	_pcnts->q = {};
}

auto h2u::_MD_Stream::ever_used() const->bool{ return !_pcnts->q.empty(); }

auto h2u::_MD_Stream::working_filepath() const->dir_t const &{
	return _working_filepath;
}

auto h2u::_MD_Stream::md_filepath() const->dir_t const &{ return _md_filepath; }
auto h2u::_MD_Stream::md_materials_dir() const->dir_t const &{ return _md_materials_dir; }

void h2u::_MD_Stream::print_and_close(){
	if(!is_open()){ return; }

	for(  std::wofstream ofs( _Wcs_to_Mbs(_md_filepath).c_str() ); !_pcnts->q.empty();  ){
		ofs << _pcnts->q.front();
		_pcnts->q.pop();
	}

	close();
}

void h2u::_MD_Stream::_push(wstring const &str){
	_pcnts->q.push(str);
}

void h2u::_MD_Stream::_push(wstring &&str){
	_pcnts->q.push( std::move(str) );
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::_MD_Stream_Guard::_MD_Stream_Guard(dir_t working_filepath)
:
	is_successful(true)
{
	for(auto &c : working_filepath){
		if(c == L'\\'){ c = L'/'; }
	}

	mdo->open( std::move(working_filepath) );
}

h2u::_MD_Stream_Guard::_MD_Stream_Guard(std::string working_filepath)
:
	_MD_Stream_Guard( _Mbs_to_Wcs(working_filepath) )
{}

h2u::_MD_Stream_Guard::~_MD_Stream_Guard(){
	if(is_successful && mdo->ever_used()){
		mdo->print_and_close();
	} else{
		std::remove( _Wcs_to_Mbs(mdo->md_filepath()).c_str() );
		mdo->close();
	}
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::md_guard::md_guard(wstring begin) : md_guard(begin, begin){}
h2u::md_guard::md_guard(wstring begin, wstring end) : _end(end){ mdo << begin; }
h2u::md_guard::~md_guard(){ mdo << _end; }
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::md_block_guard::md_block_guard(wstring s) : md_guard( wstring(L"```") + s + L"\n", L"```\n" ){}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::html_block_guard::html_block_guard(wstring const &tags){
	std::queue<wstring> q;

	for(auto itr1 = tags.cbegin(), itr2 = itr1; ; ++itr2){
		if(itr2 == tags.cend()){
			q.emplace(itr1, itr2);
			break;
		} else if(*itr2 == L' '){
			q.emplace(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	for( _end.reserve(tags.size() + 2 * q.size() + 1); !q.empty(); q.pop() ){
		auto const &tag = q.front();

		mdo << _bracket(tag);
		_end.append( _bracket(wstring{ L'/' } + tag) );
	}
}

h2u::html_block_guard::~html_block_guard(){ mdo << _end; }

auto h2u::html_block_guard::_bracket(wstring const &s)->wstring{
	return wstring{ L'<' } + s + L'>';
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

static auto _is_empty_line(wstring const &line)->bool{
	for(auto const c : line){
		if(c != L' ' && c != L'\t' && c != L'\n'){ return false; }
	}

	return true;
}

static auto _file_exists(dir_t const &filepath)->bool{
	return std::wifstream( h2u::_Wcs_to_Mbs(filepath).c_str() ).is_open();
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::_tabless_description::_tabless_description(wstring &&s)
:
	_str(  _tabless_string( std::move(s) )  )
{}

auto h2u::_tabless_description::_tabless_string(wstring &&str)->wstring{
	std::queue<wstring> qs;
	size_t total_str_len = 0;

	using str_itr_t = wstring::const_iterator;

	auto
		enqueue_f
		= [&qs, &total_str_len](str_itr_t itr1, str_itr_t itr2){
			if( !_is_empty_line({ itr1, itr2 }) ){
				while(*itr1 == L'\t'){ ++itr1; }
			}

			wstring s(itr1, itr2);
			qs.emplace( std::move(s) );
			total_str_len += std::distance(itr1, itr2);
		}
	;

	for(auto itr1 = str.cbegin(), itr2 = itr1; ; ++itr2){
		if(itr2 == str.cend()){
			enqueue_f(itr1, itr2);
			break;
		} else if(*itr2 == L'\n'){
			enqueue_f(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	for( ; !qs.empty() && _is_empty_line(qs.front()); qs.pop() ){}

	wstring res;
	res.reserve(total_str_len + 2 * qs.size());

	for(; !qs.empty(); qs.pop()){
		res.append(qs.front() + L"  \n");
	}

	return res;
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

h2u::_code_description::_code_description(wstring &&s) : _str( _Code_writing(s) ){}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

auto h2u::HTML_tag(wstring const &contents, wstring const &tag)->wstring{
	std::queue<wstring> tags;

	for(auto itr1 = tag.cbegin(), itr2 = itr1; ; ++itr2){
		if(itr2 == tag.cend()){
			tags.emplace(itr1, itr2);
			break;
		} else if(*itr2 == L' '){
			tags.emplace(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	auto
		tag_f
		= [](wstring const &s, wstring const &t){
			wstring const
				begin_str = wstring(L"<") + t + L">",
				end_str = wstring(L"</") + t + L">"
			;

			return begin_str + s + end_str;
		}
	;

	auto res = contents;

	while(!tags.empty()){
		res = tag_f(res, tags.front());
		tags.pop();
	}

	return res;
}

auto h2u::Load_image(wstring const &image_name, size_t const image_width)->wstring{
	if( !::_file_exists(mdo->md_materials_dir() + L'/' + image_name) ){
		throw std::runtime_error("Cannot find the image file in ./md_materials directory.");
	}

	auto const
		size_str
		= image_width == 0
		? wstring(L"")
		: wstring(L" width =\"") + std::to_wstring(image_width) + L"\""
	;

	return wstring(L"<img src=\"") + L"./md_materials/" + image_name + L"\"" + size_str + L">";
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

auto h2u::Empty_lines(size_t nof_el)->wstring{
	wstring const nbsp = L"&nbsp;  \n";
	wstring spaces;
	spaces.reserve(nof_el * nbsp.size());

	while(nof_el-- > 0){
		spaces.append(nbsp);
	}

	return wstring(L"\n\n") + spaces + L"\n";
}

auto h2u::Title(wstring const &title, unsigned const level)->wstring{
	wstring sharps{};

	for(auto d = level; d-- > 0; sharps += L'#'){}

	return sharps + L' ' + title + L'\n';
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

static auto Getline(std::wifstream &wis, std::wstring &wbuf)->std::wifstream &{
	static size_t constexpr String_buffer_size = 0x1'000;

	wbuf.reserve(String_buffer_size);
	wbuf.clear();

	std::wistream::sentry wse(wis, true);
	std::wstreambuf &wsbuf = *wis.rdbuf();

	auto constexpr
		cr = static_cast<wchar_t>(L'\r'),
		Lf = static_cast<wchar_t>(L'\n'),
		eof = static_cast<wchar_t>(std::wstreambuf::traits_type::eof())
	;

	while(true){
		auto const c = static_cast<wchar_t>(wsbuf.sbumpc());

		switch(c){
		case Lf:
			return wis;
		case cr:
			if( static_cast<wchar_t>(wsbuf.sgetc()) == Lf ){
				wsbuf.sbumpc();
			}

			return wis;
		case eof:
			if(wbuf.empty()){
				wis.setstate(std::ios::eofbit);
			}

			return wis;
		default:
			wbuf += c;
		}
	}
}

auto h2u::Load_code_block(wstring const code_block_tag) noexcept(false)->wstring{
	if( !::_file_exists(mdo->working_filepath()) ){
		throw std::runtime_error("the file to be loaded doesn't exist.");
	}

	std::wifstream file( _Wcs_to_Mbs(mdo->working_filepath()).c_str() );

	wstring const
		cb_begin = wstring(L"BEGIN_CODE_BLOCK(") + code_block_tag + L")",
		cb_end = wstring(L"END_CODE_BLOCK(") + code_block_tag + L")",
		cb_end2 = wstring(L"END_CODE_BLOCK_AND_LOAD(") + code_block_tag + L")"
	;

	auto
		trimmed_str_f
		= [](wstring const &s)->wstring{
			if( s.empty() || _is_empty_line(s) ){
				return s;
			}

			auto fitr = s.cbegin();
			auto bitr = std::prev(s.cend());

			while(*fitr == L' ' || *fitr == L'\t'){
				++fitr;
			}

			while(*bitr == L' ' || *bitr == L'\t'){
				--bitr;
			}

			return{ fitr, ++bitr };
		}
	;

	auto
		are_same_str_f
		= [](wstring const &s1, wstring const &s2, size_t const size){
			bool res = s1.size() >= size && s2.size() >= size;

			for(size_t i = 0; res && i < size; ++i){
				res = s1[i] == s2[i];
			}

			return res;
		}
	;

	std::queue<wstring> qs;
	size_t nof_char = 0;

	for( wstring buf; ::Getline(file, buf); ){
		if(  are_same_str_f( trimmed_str_f(buf), cb_begin, cb_begin.size() )  ){
			::Getline(file, buf);

			while(
				!are_same_str_f( trimmed_str_f(buf), cb_end, cb_end.size() )
				&& !are_same_str_f( trimmed_str_f(buf), cb_end2, cb_end2.size() )
			){
				qs.push(buf + L"\n");
				nof_char += buf.size() + 1;
				::Getline(file, buf);
			}
		}
	}

	wstring merged_str;
	merged_str.reserve(nof_char);

	while(!qs.empty()){
		merged_str.append(qs.front());
		qs.pop();
	}

	return _Code_writing(merged_str, L"cpp");
}

auto h2u::Load_description_file(wstring const &filename) noexcept(false)->wstring{
	auto const filepath = mdo->md_materials_dir() + L'/' + filename;

	if( !::_file_exists(filepath) ){
		throw std::runtime_error("Cannot find the file in ./md_materials directory.");
	}

	std::queue<wstring> qs;
	size_t nof_char = 0;
	std::wifstream file( _Wcs_to_Mbs(filepath).c_str() );

	for( wstring buf; ::Getline(file, buf); nof_char += buf.size() + 4 ){
		qs.push(buf + L"  \n");
	}

	wstring merged_str;
	merged_str.reserve(nof_char);

	while(!qs.empty()){
		merged_str.append(qs.front());
		qs.pop();
	}

	return merged_str;
}

auto h2u::_Code_writing(wstring const &str, wstring const &lang)->wstring{
	auto
		tab_count_f
		= [](wstring const &line)->size_t{
			size_t res = 0;

			for(auto const c : line){
				if(c == L'\t'){ ++res; } else{ break; }
			}

			return res;
		}
	;

	size_t constexpr max_nof_tabs = 0x1000;

	std::queue<wstring> qs;
	size_t total_str_len = 0, min_nof_tab = max_nof_tabs;

	using str_itr_t = wstring::const_iterator;

	auto
		enqueue_f
		= [&qs, &total_str_len, &min_nof_tab, tab_count_f](str_itr_t itr1, str_itr_t itr2){
			auto min_f = [](size_t _1, size_t _2) noexcept->size_t{ return _1 < _2 ? _1 : _2; };
			wstring s(itr1, itr2);

			if( !_is_empty_line(s) ){
				min_nof_tab = min_f( min_nof_tab, tab_count_f(s) );
			}

			qs.emplace( std::move(s) );
			total_str_len += std::distance(itr1, itr2);
		}
	;

	for(auto itr1 = str.cbegin(), itr2 = itr1; ; ++itr2){
		if(itr2 == str.cend()){
			enqueue_f(itr1, itr2);
			break;
		} else if(*itr2 == L'\n'){
			enqueue_f(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	while( !qs.empty() && _is_empty_line(qs.front()) ){
		qs.pop();
	}

	wstring res;
	res.reserve(8 + lang.size() + total_str_len + 2 * qs.size());

	res.append( wstring(L"```") + lang + L"\n" );

	while(!qs.empty()){
		auto const &s = qs.front();

		if( !_is_empty_line(s) ){
			res.append(s.cbegin() + min_nof_tab, s.cend());
		} else{
			res.append(s);
		}

		res.append(L"  \n");
		qs.pop();
	}

	do{
		res.pop_back();
	} while(res.back() != L'\n');

	res.append(L"```\n");

	return res;
}
