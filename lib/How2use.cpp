/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#

#include "How2use.hpp"
#include <cassert>
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

static char32_t constexpr Escape_base = 0xDC80;

static auto _is_surrogate(char32_t const cp)->bool{ return cp >= 0xD800 && cp <= 0xDFFF; }

static auto _is_escape(char32_t const cp)->bool{
	return cp >= Escape_base && cp <= Escape_base + 0x7F;
}
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

namespace{
	namespace inner{
		template<class CON>
		class No_Reallocation;
	}
}

template<class CON>
class inner::No_Reallocation{
public:
	No_Reallocation(CON const &con) noexcept : _con(con), _capa(con.capacity()){}
	~No_Reallocation(){ assert(_con.capacity() == _capa); }

private:
	CON const &_con;
	std::size_t _capa;
};
//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$//--//--//--//--//-$

static void _Push_code_point(wstring &wcs, char32_t const cp){
	inner::No_Reallocation<wstring> const nr(wcs);
	
	if( sizeof(wchar_t) == 2 && cp > 0xFFFF ){
		wcs.push_back(   static_cast<wchar_t>(  0xD800 + ( (cp - 0x10000) >> 10 )  )   );
		wcs.push_back(   static_cast<wchar_t>(  0xDC00 + ( (cp - 0x10000) & 0x3FF )  )   );
	}
	else{
		wcs.push_back( static_cast<wchar_t>(cp) );
	}
}

static void _Push_utf8(std::string &bytes, char32_t const cp){
	inner::No_Reallocation<std::string> const nr(bytes);
	auto const byte_f = [](char32_t const c)->char{ return static_cast<char>(c); };

	if( _is_escape(cp) ){
		bytes.push_back( byte_f(cp - Escape_base + 0x80) );
	}
	else if(cp < 0x80){
		bytes.push_back( byte_f(cp) );
	}
	else if(cp < 0x800){
		bytes.push_back( byte_f(0xC0 | cp >> 6) );
		bytes.push_back(  byte_f( 0x80 | (cp & 0x3F) )  );
	}
	else if(cp < 0x10000){
		bytes.push_back( byte_f(0xE0 | cp >> 12) );
		bytes.push_back(   byte_f(  0x80 | ( (cp >> 6) & 0x3F )  )   );
		bytes.push_back(  byte_f( 0x80 | (cp & 0x3F) )  );
	}
	else{
		bytes.push_back( byte_f(0xF0 | cp >> 18) );
		bytes.push_back(   byte_f(  0x80 | ( (cp >> 12) & 0x3F )  )   );
		bytes.push_back(   byte_f(  0x80 | ( (cp >> 6) & 0x3F )  )   );
		bytes.push_back(  byte_f( 0x80 | (cp & 0x3F) )  );
	}
}

static auto _Utf8_length(char32_t const cp)->size_t{
	if( _is_escape(cp) || cp < 0x80 ){
		return 1;
	}
	else if(cp < 0x800){
		return 2;
	}
	else if(cp < 0x10000){
		return 3;
	}
	else{
		return 4;
	}
}

struct _Code_unit{
	char32_t code_point;
	size_t width;
};

static auto _Code_unit_at(wstring const &wcs, size_t const i)->_Code_unit{
	auto const cp = static_cast<char32_t>(wcs[i]);

	bool const  
		is_pair
		= sizeof(wchar_t) == 2 && cp >= 0xD800 && cp <= 0xDBFF && i + 1 < wcs.size()
		&& wcs[i + 1] >= 0xDC00 && wcs[i + 1] <= 0xDFFF
	;

	if( _is_escape(cp) ){
		return{ cp, 1 };
	}
	else if(is_pair){
		auto const low = static_cast<char32_t>(wcs[i + 1]);

		return{ 0x10000 + ( (cp - 0xD800) << 10 ) + (low - 0xDC00), 2 };
	}
	else if( _is_surrogate(cp) || cp > 0x10FFFF ){
		return{ 0xFFFD, 1 };
	}
	else{
		return{ cp, 1 };
	}
}

static auto _Utf8_to_Wcs(std::string const &bytes)->wstring{
	wstring res;
	res.reserve(bytes.size());

	for(size_t i = 0; i < bytes.size();){
		auto const lead = static_cast<unsigned char>(bytes[i]);

		size_t const  
			nof_trail
			= lead < 0x80 ? 0
			: lead >> 5 == 0x06 ? 1
			: lead >> 4 == 0x0E ? 2
			: lead >> 3 == 0x1E ? 3
			: 4
		;

		char32_t const min_cp = nof_trail == 1 ? 0x80 : nof_trail == 2 ? 0x800 : 0x10000;
		char32_t cp = static_cast<char32_t>( lead & (0x7F >> nof_trail) );
		bool is_valid = nof_trail < 4 && i + nof_trail < bytes.size();

		for(size_t k = 1; is_valid && k <= nof_trail; ++k){
			auto const trail = static_cast<unsigned char>(bytes[i + k]);

			is_valid = trail >> 6 == 0x02;
			cp = cp << 6 | (trail & 0x3F);
		}

		is_valid
		= is_valid && (nof_trail == 0 || cp >= min_cp) && cp <= 0x10FFFF && !_is_surrogate(cp);

		if(is_valid){
			_Push_code_point(res, cp);
			i += nof_trail + 1;
		}
		else{
			_Push_code_point(res, Escape_base + lead - 0x80);
			++i;
		}
	}

	return res;
}

static auto _Wcs_to_Utf8(wstring const &wcs)->std::string{
	size_t const  
		nof_byte
		= [&wcs]()->size_t{
			size_t res = 0;

			for(size_t i = 0; i < wcs.size();){
				auto const unit = _Code_unit_at(wcs, i);

				res += _Utf8_length(unit.code_point);
				i += unit.width;
			}

			return res;
		}()
	;

	std::string res;
	res.reserve(nof_byte);

	for(size_t i = 0; i < wcs.size();){
		auto const unit = _Code_unit_at(wcs, i);

		_Push_utf8(res, unit.code_point);
		i += unit.width;
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

auto h2u::_MD_Stream::is_open() const->bool{ return _md_filepath != dir_t(); }

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

	for(  std::ofstream ofs( _Wcs_to_Mbs(_md_filepath).c_str() ); !_pcnts->q.empty();  ){
		ofs << _Wcs_to_Utf8(_pcnts->q.front());
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
: is_successful(true){
	for(auto &c : working_filepath){
		if(c == L'\\'){ c = L'/'; }
	}

	mdo->open( std::move(working_filepath) );
}

h2u::_MD_Stream_Guard::_MD_Stream_Guard(std::string working_filepath)
: _MD_Stream_Guard( _Mbs_to_Wcs(working_filepath) ){}

h2u::_MD_Stream_Guard::~_MD_Stream_Guard(){
	if(is_successful && mdo->ever_used()){
		mdo->print_and_close();
	}
	else{
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
		}
		else if(*itr2 == L' '){
			q.emplace(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	for( _end.reserve(tags.size() + 2 * q.size() + 1); !q.empty(); q.pop() ){
		auto const &tag = q.front();

		mdo << _Bracket(tag);
		_end.append( _Bracket(wstring{ L'/' } + tag) );
	}
}

h2u::html_block_guard::~html_block_guard(){ mdo << _end; }

auto h2u::html_block_guard::_Bracket(wstring const &s)->wstring{
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
: _str(  _Tabless_string( std::move(s) )  ){}

auto h2u::_tabless_description::_Tabless_string(wstring &&str)->wstring{
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
		}
		else if(*itr2 == L'\n'){
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
		}
		else if(*itr2 == L' '){
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

static auto Getline(std::ifstream &is, wstring &wbuf)->std::ifstream &{
	std::string buf;

	if( std::getline(is, buf) ){
		if(!buf.empty() && buf.back() == '\r'){
			buf.pop_back();
		}

		wbuf = _Utf8_to_Wcs(buf);
	}

	return is;
}

auto h2u::Load_code_block(wstring const code_block_tag) noexcept(false)->wstring{
	if( !::_file_exists(mdo->working_filepath()) ){
		throw std::runtime_error("the file to be loaded doesn't exist.");
	}

	std::ifstream file( _Wcs_to_Mbs(mdo->working_filepath()).c_str() );

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
			while(true){
				if( !::Getline(file, buf) ){
					throw std::runtime_error("END_CODE_BLOCK is missing.");
				}
				else if(
					are_same_str_f( trimmed_str_f(buf), cb_end, cb_end.size() )
					|| are_same_str_f( trimmed_str_f(buf), cb_end2, cb_end2.size() )
				){
					break;
				}

				qs.push(buf + L"\n");
				nof_char += buf.size() + 1;
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
	std::ifstream file( _Wcs_to_Mbs(filepath).c_str() );

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
		}
		else if(*itr2 == L'\n'){
			enqueue_f(itr1, itr2);
			itr1 = itr2 + 1;
		}
	}

	while( !qs.empty() && _is_empty_line(qs.front()) ){
		qs.pop();
	}

	wstring const header = wstring(L"```") + lang + L"\n";
	wstring res;

	res.reserve(8 + lang.size() + total_str_len + 2 * qs.size());
	res.append(header);

	while(!qs.empty()){
		auto const &s = qs.front();

		if( !_is_empty_line(s) ){
			res.append(s.cbegin() + min_nof_tab, s.cend());
		}
		else{
			res.append(s);
		}

		res.append(L"  \n");
		qs.pop();
	}

	if(res.size() > header.size()){
		do{
			res.pop_back();
		}
		while(res.back() != L'\n');
	}

	res.append(L"```\n");

	return res;
}
