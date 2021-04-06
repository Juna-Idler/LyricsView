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

//タイムタグ文字列を返すときの小数点文字
extern const wchar_t &DecimalPoint;

//DecimalPointを.にします
void SetDecimalPointPeriod(void);
//DecimalPointを:にします
void SetDecimalPointColon(void);


//タイムタグ文字列からミリ秒に変換します
int timetag2milisec( const wchar_t *timetag);
int timetag2milisec( const std::wstring &timetag);

//タイムタグ文字列からミリ秒に変換します
//タイムタグの長さ(10or7or0)も得られます
int timetag2milisec_length(size_t &taglength, const wchar_t *timetag);
int timetag2milisec_length(size_t &taglength, const std::wstring &timetag);

//ミリ秒をタイムタグ文字列に変換します
//内部的なバッファにある文字列を返します
const wchar_t *milisec2timetag(int milisec);
//ミリ秒をタイムタグ文字列に変換します
//引数のwstringに格納します
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

//行頭タイムタグ行を時間と文字列に分離する
void SeparateHeadTimeTagLine(Pair &dest, const wchar_t *line ,size_t length);
//行頭タイムタグ行を時間と文字列に分離する
void SeparateHeadTimeTagLine(Pair &dest, const std::wstring &line );

//Pickupと同じ？
void SeparateHeadTimeTag(PairContainer &dest, const std::wstring &text );

//行頭タイムタグの付いた行を抽出する
void PickupHeadTimeTagLine(PairContainer &dest, const std::wstring &text );
void PickupHeadTimeTagLine(PairContainer &dest, const std::vector<std::wstring> &lines );


//カラオケタグ行を分解する
void SeparateKaraokeLine(PairContainer &dest, const wchar_t *line );
//行頭タグだけ既に分離していた場合
void SeparateKaraokeLine(PairContainer &dest, int head_milisec, const wchar_t *line );

//カラオケタグを全行分解する
void SeparateKaraokeText(std::vector<PairContainer> &dest, const std::wstring &text );

//タイムタグを除去してdestに格納する
size_t RemoveTimeTag(std::wstring &dest,const wchar_t *text);
//タイムタグを除去する
size_t RemoveTimeTag(std::wstring &text);

//行頭以外のタイムタグを除去する
void RemoveKaraokeTag( std::wstring &dest,const std::wstring &text );

}//namespace TimeTag


}//namespace Lyrics
}//namespace Juna

