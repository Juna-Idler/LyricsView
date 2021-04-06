#pragma once

#include <string>
#include <vector>
//#include <list>

namespace Juna {


//ルビ指定語句
struct RubyingWord
{
//ルビ対象検索文字列
	std::wstring target;
//検索文字列内の親文字の開始位置
	unsigned short parent_offset;
//親文字の長さ
	unsigned short parent_length;
//ルビ文字
	std::wstring ruby;


	RubyingWord(void) : parent_offset(0), parent_length(0) {}
	RubyingWord(const std::wstring &target,const std::wstring &ruby,unsigned short offset = 0,unsigned short length = 0)
		: target(target), ruby(ruby), parent_offset(offset), parent_length(length)
	{
		if (offset >= target.size())
			parent_offset = 0;
		if (length == 0 || unsigned(parent_offset + length) > target.size())
			parent_length = (unsigned short)target.size() - parent_offset;
	}
};

//ルビを含む文字列
struct RubyString
{
	struct RubyPosition
	{
		unsigned short parent_offset;
		unsigned short parent_length;

		unsigned short phonetic_offset;
		unsigned short phonetic_length;

		void Set(unsigned short parent_o,unsigned short parent_l,unsigned short phonetic_o,unsigned short phonetic_l)
		{
			this->parent_offset = parent_o;
			this->parent_length = parent_l;
			this->phonetic_offset = phonetic_o;
			this->phonetic_length = phonetic_l;
		}
	};
	typedef std::vector<RubyPosition> RubyPositions;

private:
	std::wstring parent;
	std::wstring phonetic;
	RubyPositions positions;

public:

	RubyString(void) {}
	RubyString(const std::wstring &source) {AssignString(source);}
	RubyString(const std::wstring &source,
			   const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark)
	{AssignString(source,parent_mark,begin_mark,end_mark);}

//ルビなし返す
	const std::wstring &GetString(void) const {return parent;}
	const std::wstring &GetParent(void) const {return parent;}

//"(paretn)親文字(begin)おやもじ(end)"なフォーマットで出力
	const std::wstring GetString(const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark) const;


//親文字の位置にルビを置き換えた文字列
	const std::wstring &GetPhonetic(void) const {return phonetic;}

	const RubyPositions &GetRubyPositions(void) const {return positions;}
	const RubyPosition &GetRubyPosition(size_t i) const {return positions[i];}

	bool Empty(void) const {return parent.empty();}

	void Clear(void) {parent.clear();phonetic.clear();positions.clear();}
	void AssignString(const std::wstring &source)
	{
		positions.clear();
		parent = phonetic = source;
	}
//"(paretn)親文字(begin)おやもじ(end)"なフォーマットのソースから入力
	size_t AssignString(const std::wstring &source,
						const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark);

//textから可能な限りwordを探してルビを追加　親文字に他のルビが被っていたら追加しない
	size_t AddRuby(const RubyingWord &word);


	struct RubyInfo
	{
		size_t parent_mark;
		size_t parent;
		size_t begin_mark;
		size_t ruby;
		size_t end_mark;
		size_t endend;

		RubyInfo(size_t pm,size_t p,size_t bm,size_t r,size_t em,size_t ee) :
			parent_mark(pm),parent(p),begin_mark(bm),ruby(r),end_mark(em),endend(ee)
		{}
	};

//(paretn)親文字(begin)おやもじ(end)"なフォーマットのソースからルビ指定文字の位置情報を得る
	static size_t GetRubyInfo(std::vector<RubyInfo> &dest,
						const std::wstring &source,
						const std::wstring &parent,const std::wstring &begin,const std::wstring &end);

};


}//namespace Juna




