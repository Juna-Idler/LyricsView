#pragma once

#include <string>
#include <vector>
//#include <list>

namespace Juna {


//���r�w����
struct RubyingWord
{
//���r�Ώی���������
	std::wstring target;
//������������̐e�����̊J�n�ʒu
	unsigned short parent_offset;
//�e�����̒���
	unsigned short parent_length;
//���r����
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

//���r���܂ޕ�����
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

//���r�Ȃ��Ԃ�
	const std::wstring &GetString(void) const {return parent;}
	const std::wstring &GetParent(void) const {return parent;}

//"(paretn)�e����(begin)�������(end)"�ȃt�H�[�}�b�g�ŏo��
	const std::wstring GetString(const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark) const;


//�e�����̈ʒu�Ƀ��r��u��������������
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
//"(paretn)�e����(begin)�������(end)"�ȃt�H�[�}�b�g�̃\�[�X�������
	size_t AssignString(const std::wstring &source,
						const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark);

//text����\�Ȍ���word��T���ă��r��ǉ��@�e�����ɑ��̃��r������Ă�����ǉ����Ȃ�
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

//(paretn)�e����(begin)�������(end)"�ȃt�H�[�}�b�g�̃\�[�X���烋�r�w�蕶���̈ʒu���𓾂�
	static size_t GetRubyInfo(std::vector<RubyInfo> &dest,
						const std::wstring &source,
						const std::wstring &parent,const std::wstring &begin,const std::wstring &end);

};


}//namespace Juna




