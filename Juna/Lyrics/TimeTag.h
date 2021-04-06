#pragma once

#include <string>
#include <vector>

namespace Juna {
namespace Lyrics {

namespace TimeTag {


const int MinTime = 0;
extern const std::wstring &MinTimeTag;

const int MaxTime  = (99 * 60 + 59) * 1000 + 99 * 10;
extern const std::wstring &MaxTimeTag;

//�^�C���^�O�������Ԃ��Ƃ��̏����_����
extern const wchar_t &DecimalPoint;

//DecimalPoint��.�ɂ��܂�
void SetDecimalPointPeriod(void);
//DecimalPoint��:�ɂ��܂�
void SetDecimalPointColon(void);


//�^�C���^�O�����񂩂�~���b�ɕϊ����܂�
int timetag2milisec( const wchar_t *timetag);
int timetag2milisec( const std::wstring &timetag);

//�^�C���^�O�����񂩂�~���b�ɕϊ����܂�
//�^�C���^�O�̒���(10or7or0)�������܂�
int timetag2milisec_length(size_t &taglength, const wchar_t *timetag);
int timetag2milisec_length(size_t &taglength, const std::wstring &timetag);

//�~���b���^�C���^�O������ɕϊ����܂�
//�����I�ȃo�b�t�@�ɂ��镶�����Ԃ��܂�
const wchar_t *milisec2timetag(int milisec);
//�~���b���^�C���^�O������ɕϊ����܂�
//������wstring�Ɋi�[���܂�
void milisec2timetag(std::wstring &timetag, int milisec);


struct Pair
{
public:
	int milisec;
	const wchar_t *GetTimeTag(void) const {return milisec2timetag(milisec);}
	void SetTimeTag( const wchar_t *timetag) {milisec = timetag2milisec(timetag);}
	void SetTimeTag( const std::wstring &timetag) {milisec = timetag2milisec(timetag);}

	std::wstring word;

	Pair(void) : milisec(-1) , word() {}
	Pair( int milisec , const std::wstring &word ) : milisec(milisec) , word(word) {}
	Pair( const wchar_t *tt , const std::wstring &word ) : milisec(timetag2milisec( tt )) , word(word) {}

	Pair( int milisec , const wchar_t *word , size_t wordlength) : milisec(milisec) , word(word,wordlength)  {}
	Pair( const wchar_t *tt , const wchar_t *word , size_t wordlength) : milisec(timetag2milisec( tt )) , word(word,wordlength) {}
};
typedef std::vector<Pair> PairContainer;

//�s���^�C���^�O�s�����Ԃƕ�����ɕ�������
void SeparateHeadTimeTagLine(Pair &dest, const wchar_t *line ,size_t length);
//�s���^�C���^�O�s�����Ԃƕ�����ɕ�������
void SeparateHeadTimeTagLine(Pair &dest, const std::wstring &line );

//Pickup�Ɠ����H
void SeparateHeadTimeTag(PairContainer &dest, const std::wstring &text );

//�s���^�C���^�O�̕t�����s�𒊏o����
void PickupHeadTimeTagLine(PairContainer &dest, const std::wstring &text );
void PickupHeadTimeTagLine(PairContainer &dest, const std::vector<std::wstring> &lines );


//�J���I�P�^�O�s�𕪉�����
void SeparateKaraokeLine(PairContainer &dest, const wchar_t *line );
//�s���^�O�������ɕ������Ă����ꍇ
void SeparateKaraokeLine(PairContainer &dest, int head_milisec, const wchar_t *line );

//�J���I�P�^�O��S�s��������
void SeparateKaraokeText(std::vector<PairContainer> &dest, const std::wstring &text );

//�^�C���^�O����������dest�Ɋi�[����
size_t RemoveTimeTag(std::wstring &dest,const wchar_t *text);
//�^�C���^�O����������
size_t RemoveTimeTag(std::wstring &text);

//�s���ȊO�̃^�C���^�O����������
void RemoveKaraokeTag( std::wstring &dest,const std::wstring &text );

}//namespace TimeTag


}//namespace Lyrics
}//namespace Juna

