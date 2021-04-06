
/*!
	@file
	@brief 侵入型双方向連結リストコンテナ

	特徴
	- コンテナを介さずに要素をリストから外すことができる。
	- 要素はコピー可能でなくても構わない。
	- 内部でメモリ確保操作をしない。
	- ポインタのリストより少しだけメモリ効率がいい。

	- intrusive_list_nodeを継承しなければならない。
	- 要素自体は別の安定した場所で確保しなければならない。

	@author Juna
*/

#ifndef Juna_Intrusive_Linked_List_Hpp
#define Juna_Intrusive_Linked_List_Hpp


#include <stddef.h>
#include <iterator>
#include <assert.h>


//!	namespace Juna
namespace Juna {

//!	リスト要素の侵入部分の基礎部分
/*!
*/
class base_intrusive_list_node
{
protected:
//!	デフォルトコンストラクタ
/*!
	@post linked() == false
*/
	base_intrusive_list_node(void) {next = prev = this;}
//!	デストラクタ
/*!
	@pre assert(linked() == false)
*/
	~base_intrusive_list_node()
	{
		assert(!linked());	//繋がったまま消えるな
	}

	//
//!	リストに繋がっているか
/*!
	@retval true 繋がっている
	@retval false 繋がっていない
*/
	bool linked(void) const {return next != this;}

//!	リストから外れる
/*!
	リストを介さずにリストから外れる。
*/
	void unlink(void)
	{
		prev->next = next;
		next->prev = prev;
		next = prev = this;
	}
private:
//	this->next == &otherとなるように繋がる。
/*	基本的にどちらかが未接続の状態で使用する。 */
	void link_next(base_intrusive_list_node &other)
	{
		other.prev->next = next;
		next->prev = other.prev;
		other.prev = this;
		next = &other;
	}
//	入れ替え
/*	otherとリンク（位置）を入れ替える。 */
	void swap(base_intrusive_list_node &other)
	{
		base_intrusive_list_node *this_next = this->next;
		base_intrusive_list_node *this_prev = this->prev;
		base_intrusive_list_node *other_next = other.next;
		base_intrusive_list_node *other_prev = other.prev;

		this_prev->next = this_next->prev = &other;
		other_prev->next = other_next->prev = this;

		base_intrusive_list_node *next = this->next;
		base_intrusive_list_node *prev = this->prev;
		this->next = other.next;
		this->prev = other.prev;
		other.next = next;
		other.prev = prev;
	}
private:
friend class base_intrusive_list;

	base_intrusive_list_node *next;
	base_intrusive_list_node *prev;

private:
	base_intrusive_list_node(base_intrusive_list_node &);
	void operator=(base_intrusive_list_node &);
};

//!	リスト要素の侵入部分
/*!
	これを継承することでリストの要素となることができる。
	テンプレート引数は、要素が複数のリストに登録される場合に利用する。
*/
template<int TagNumber>
class intrusive_list_node_ : public base_intrusive_list_node {};

//!	リスト要素の侵入部分
/*!
	単一の要素のための別名
*/
typedef intrusive_list_node_<0> intrusive_list_node;

/*===========================================================================*/


//	侵入型双方向リストコンテナクラスの共通部分
//	base_intrusive_list_nodeのfriend
class base_intrusive_list
{
protected:
	typedef base_intrusive_list_node ilist_node;
	typedef size_t size_type;

	base_intrusive_list(void) : header() {}
	~base_intrusive_list() {}

	void clear(void);

	static ilist_node *insert(ilist_node *pos,ilist_node *ins);
	static ilist_node *insert_after(ilist_node *pos,ilist_node *ins);
	static ilist_node *erase(ilist_node *pos);

	static void splice(ilist_node *pos,ilist_node &other_header);
	static void splice(ilist_node *pos,ilist_node *i);
	static void splice(ilist_node *pos,ilist_node *first,ilist_node *last);

	size_type size(void) const;
	bool empty(void) const {return !header.linked();}

	void swap(base_intrusive_list &other) {header.swap(other.header);}

	static ilist_node *next(const ilist_node *node) {return node->next;}
	static ilist_node *prev(const ilist_node *node) {return node->prev;}

	class list_header : public base_intrusive_list_node
	{
	public:
		list_header(void) : base_intrusive_list_node() {}
		~list_header() {}
	};
	mutable list_header header;

private:
	base_intrusive_list(base_intrusive_list &);
	void operator=(base_intrusive_list &);
};

//	base_intrusive_list実装

inline void base_intrusive_list::clear(void)
{
	for (ilist_node *list = header.next;list != &header;list = erase(list))
		;
}

inline base_intrusive_list_node *
	base_intrusive_list::insert(ilist_node *pos,ilist_node *ins)
{
	assert(!ins->linked());
	ins->link_next(*pos);
	return ins;
}
inline base_intrusive_list_node *
	base_intrusive_list::insert_after(ilist_node *pos,ilist_node *ins)
{
	assert(!ins->linked());
	pos->link_next(*ins);
	return ins;
}

inline base_intrusive_list_node *
	base_intrusive_list::erase(ilist_node *pos)
{
	ilist_node *ret = pos->next;
	pos->unlink();
	return ret;
}

inline void
	base_intrusive_list::splice(ilist_node *pos,ilist_node &other_header)
{
	ilist_node *next = other_header.next;
	ilist_node *prev = other_header.prev;

	prev->next = pos;
	next->prev = pos->prev;

	pos->prev->next = other_header.next;
	pos->prev = other_header.prev;

	other_header.next = other_header.prev = &other_header;
}
inline void base_intrusive_list::splice(ilist_node *pos,ilist_node *i)
{
	assert(pos != i);
	i->prev->next = i->next;
	i->next->prev = i->prev;
	i->prev = pos->prev;
	i->next = pos;
	pos->prev->next = i;
	pos->prev = i;
}
inline void base_intrusive_list::splice(ilist_node *pos,
										ilist_node *first,ilist_node *last)
{
	assert(first != last);
	ilist_node *pos_prev = pos->prev;

	pos->prev->next = first;
	pos->prev = last->prev;

	last->prev->next = pos;
	last->prev = first->prev;

	first->prev->next = last;
	first->prev = pos_prev;
}
inline base_intrusive_list::size_type base_intrusive_list::size(void) const
{
	size_type count = 0;
	for (const ilist_node *ln = header.next;ln != &header;ln = ln->next)
		count++;
	return count;
}

//	侵入型双方向リストコンテナクラスのタグの振り分け部分
template<int TagNumber>
class base_intrusive_list_ : public base_intrusive_list
{
protected:
	typedef base_intrusive_list base_class;
	typedef intrusive_list_node_<TagNumber> ilist_node;
	typedef base_class::size_type size_type;

	static ilist_node *insert(ilist_node *pos,ilist_node *ins)
		{return static_cast<ilist_node *>(base_class::insert(pos,ins));}
	static ilist_node *insert_after(ilist_node *pos,ilist_node *ins)
		{return static_cast<ilist_node *>(base_class::insert_after(pos,ins));}
	static ilist_node *erase(ilist_node *pos)
		{return static_cast<ilist_node *>(base_class::erase(pos));}

	static void splice(ilist_node *pos,ilist_node &other_header)
		{base_class::splice(pos,other_header);}
	static void splice(ilist_node *pos,ilist_node *i)
		{base_class::splice(pos,i);}
	static void splice(ilist_node *pos,ilist_node *first,ilist_node *last)
		{base_class::splice(pos,first,last);}

	void swap(base_intrusive_list_ &other) {base_class::swap(other);}

	static ilist_node *next(const ilist_node *node)
		{return static_cast<ilist_node *>(base_class::next(node));}
	static ilist_node *prev(const ilist_node *node)
		{return static_cast<ilist_node *>(base_class::prev(node));}
};

/*===========================================================================*/

//!	マージ
template <class IntrusiveListT,class BinaryPredicate>
void intrusive_list_merge(IntrusiveListT &list,IntrusiveListT &other,BinaryPredicate less);
//!	マージソート
template <int sort_stack_size,class IntrusiveListT,class BinaryPredicate>
void intrusive_list_sort(IntrusiveListT &list,BinaryPredicate less);


/*===========================================================================*/

//!	侵入型双方向リストコンテナクラステンプレート
/*!
	@sa intrusive_list.hpp
*/
template<class DerivedNode,int TagNumber = 0>
class intrusive_list : private base_intrusive_list_<TagNumber>
{
	typedef base_intrusive_list_<TagNumber> base_class;
	typedef base_class::ilist_node ilist_node;
	typedef base_class::size_type size_type;
	typedef DerivedNode			node_type;
	typedef DerivedNode *		pointer;
	typedef const DerivedNode *	const_pointer;
	typedef DerivedNode &		reference;
	typedef const DerivedNode &	const_reference;

//	順イテレータ用ポリシー
	struct normal_step
	{
		template<class T> static T *inc(T *node) {return next(node);}
		template<class T> static T *dec(T *node) {return prev(node);}
	};
//	逆イテレータ用ポリシー
	struct reverse_step
	{
		template<class T> static T *inc(T *node) {return prev(node);}
		template<class T> static T *dec(T *node) {return next(node);}
	};
//	nodeポインタ操作権を与える
	friend struct normal_step;
	friend struct reverse_step;

//	イテレータの実装
	template<class StepPolicy>
	class base_const_iterator
	{
	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		typedef ptrdiff_t distance_type;

	protected: friend class intrusive_list<node_type,TagNumber>;
		base_const_iterator(pointer p) : node(p) {}
	public:
		base_const_iterator(void) : node(0) {}
		template<class S>
		base_const_iterator(base_const_iterator<S> &src) : node(src.get()) {}

		reference operator*(void) const {return *node;}
		pointer operator->(void) const {return node;}

		base_const_iterator operator++(void) {increment();return *this;}
		base_const_iterator operator++(int) {base_const_iterator r = *this;increment();return r;}
		base_const_iterator operator--(void) {decrement();return *this;}
		base_const_iterator operator--(int) {base_const_iterator ret = *this;decrement();return ret;}

		template<class S>
		bool operator==(const base_const_iterator<S> &rhs) const {return node == rhs.get();}
		template<class S>
		bool operator!=(const base_const_iterator<S> &rhs) const {return node != rhs.get();}

		pointer get(void) const {return node;}

	protected:
		void increment(void) {node = StepPolicy().inc(node);}
		void decrement(void) {node = StepPolicy().dec(node);}
		pointer node;
	};

	template<class StepPolicy>
	class base_iterator : public base_const_iterator<StepPolicy>
	{
	private: friend class intrusive_list<node_type,TagNumber>;
		base_iterator(pointer p) : base_const_iterator<StepPolicy>(p) {}
	public:
		base_iterator(void) {}
		template<class S>
		base_iterator(base_iterator<S> &src) : base_const_iterator(src) {}

		base_iterator operator++(void) {this->increment();return *this;}
		base_iterator operator++(int) {base_iterator r = *this;this->increment();return r;}
		base_iterator operator--(void) {this->decrement();return *this;}
		base_iterator operator--(int) {base_iterator ret = *this;this->decrement();return ret;}
	};
//	便利関数
	static pointer next(const ilist_node *node) {return static_cast<pointer>(base_class::next(node));}
	static pointer prev(const ilist_node *node) {return static_cast<pointer>(base_class::prev(node));}
	static pointer cast(ilist_node *node) {return static_cast<pointer>(node);}
	static pointer next(const_pointer node) {return static_cast<pointer>(base_class::next(node));}
	static pointer prev(const_pointer node) {return static_cast<pointer>(base_class::prev(node));}

	ilist_node &header(void) const {return static_cast<ilist_node &>(static_cast<base_intrusive_list_node &>(base_class::header));}

public:
//!	イテレータ (bidirectional_iterator)
	typedef base_iterator<normal_step> iterator;
//!	コンストイテレータ (bidirectional_iterator)
	typedef base_const_iterator<normal_step> const_iterator;
//!	リバースイテレータ
	typedef base_iterator<reverse_step> reverse_iterator;
//!	コンストリバースイテレータ
	typedef base_const_iterator<reverse_step> const_reverse_iterator;

//!	デフォルトコンストラクタ
/*!	これ以外にコンストラクタは無い */
	intrusive_list(void) : base_intrusive_list_<TagNumber>() {}
//!	デストラクタ
/*!	clear()を呼ぶ */
	~intrusive_list() {clear();}

//!	@name 全要素排除
//@{
//!	全要素排除
/*!	このリストの全要素をリストから排除する。 */
	void clear(void) {base_class::clear();}
//@}

//!	@name 要素数
//@{
//!	要素数
/*!	いちいちリストをたどって数えます。 */
	size_type size(void) const {return base_class::size();}
//!	リストが空かどうか
/*!	size()より速い size() == 0 */
	bool empty(void) const {return base_class::empty();}
//@}

//!	@name 端の要素参照
//@{
//!	先頭要素参照
/*! @pre empty() == false */
	reference front(void) {return *next(&header());}
//!	先頭要素コンスト参照
/*! @pre empty() == false */
	reference front(void) const {return *next(&header());}

//!	末尾要素参照
/*! @pre empty() == false */
	reference back(void) {return *prev(&header());}
//!	末尾要素コンスト参照
/*! @pre empty() == false */
	reference back(void) const {return *prev(&header());}
//@}

//!	@name スタック操作
//@{
//!	先頭へのpush
/*!
	リストの先頭にinsを挿入する。
	@pre assert(ins.linked() == false)
*/
	void push_front(reference ins) {base_class::insert_after(&header(),&ins);}
//!	末尾へのpush
/*!
	リストの末尾にinsを挿入する。
	@pre assert(ins.linked() == false)
*/
	void push_back(reference ins) {base_class::insert(&header(),&ins);}

//!	先頭要素のpop
/*!
	先頭要素を排除する。
	@pre empty() == false
*/
	void pop_front(void) {base_class::erase(next(&header()));}
//!	末尾要素のpop
/*!
	末尾要素を排除する。
	@pre empty() == false
*/
	void pop_back(void) {base_class::erase(prev(&header()));}
//@}

//!	@name イテレータ
//@{
//!	先頭イテレータ
	iterator begin(void) {return next(&header());}
//!	コンスト先頭イテレータ
	const_iterator begin(void) const {return next(&header());}
//!	終端イテレータ
	iterator end(void) {return cast(&header());}
//!	コンスト終端イテレータ
	const_iterator end(void) const {return cast(&header());}
//!	逆先頭イテレータ
	reverse_iterator rbegin(void) {return prev(&header());}
//!	コンスト逆先頭イテレータ
	const_reverse_iterator rbegin(void) const {return prev(&header());}
//!	逆終端イテレータ
	reverse_iterator rend(void) {return cast(&header());}
//!	コンスト逆終端イテレータ
	const_reverse_iterator rend(void) const {return cast(&header());}
//@}


//	挿入、排除、繋ぎ替え
//	位置指定イテレータにしか依存しないから、変な感じがするが全てstatic
//	何かよく判らないけどイテレータを参照で受けないと「fatal error C1001」が起きることが
//	どっちにしてもVC6だろうけど。

//!	@name 挿入
//@{
//!	要素の挿入
/*!
	posの位置にinsを挿入する。
	@return 挿入した要素を指すイテレータ
	@pre assert(ins.linked() == false)
*/
	iterator insert(iterator pos,reference ins)
		{return cast(base_class::insert(pos.get(),&ins));}
//!	要素の挿入
/*!
	posの次の位置にinsを挿入する。
	@return 挿入した要素を指すイテレータ
	@pre assert(ins.linked() == false)
*/
	iterator insert_after(iterator pos,reference ins)
		{return cast(base_class::insert_after(pos.get(),&ins));}
//@}

//!	@name 排除
//@{
//!	要素の排除
/*!
	posの要素をリストから排除する。
	@return 削除した要素の次の要素を指すイテレータ
	@post posの要素への参照は有効。
*/
	iterator erase(iterator pos)
		{return cast(base_class::erase(pos.get()));}
//!	要素の排除
/*!
	posの次の要素をリストから排除する。
	@return 削除した要素の次の要素を指すイテレータ
*/
	iterator erase_after(iterator pos)
		{return cast(base_class::erase(next(pos.get())));}
//@}

//!	@name 要素の繋ぎ替え
//@{
//!	要素の繋ぎ替え
/*!
	lの全要素をposの位置に繋ぎ替える。lは空になる。
*/
	void splice(iterator pos,intrusive_list &l)
		{base_class::splice(pos.get(),l.header());}
//!	要素の繋ぎ替え
/*!
	iの要素をposの位置に繋ぎ替える。
	@pre assert(pos != i)
*/
	void splice(iterator pos,intrusive_list &,iterator i)
		{splice(pos,i);}
//!	要素の繋ぎ替え
/*!
	[first-last)の範囲にある要素をposの位置に繋ぎ替える。
	@pre assert(first != last)
	@pre first-last間にposがあってはならない。
*/
	void splice(iterator pos,intrusive_list &,iterator first,iterator last)
		{splice(pos,first,last);}
//@}

//!	@name 要素の繋ぎ替え
//@{
//!	要素の繋ぎ替え
/*!
	iの要素をposの位置に繋ぎ替える。
	@pre assert(pos != i)
	互換用のいらない引数が無い版
*/
	static void splice(iterator pos,iterator i)
		{base_class::splice(pos.get(),i.get());}
//!	要素の繋ぎ替え
/*!
	[first-last)の範囲にある要素をposの位置に繋ぎ替える。
	@pre assert(first != last)
	@pre first-last間にposがあってはならない。
	互換用のいらない引数が無い版
*/
	static void splice(iterator pos,iterator first,iterator last)
		{base_class::splice(pos.get(),first.get(),last.get());}


//!	@name アルゴリズム
//@{
//!	リスト間の要素の全交換
/*!	lと要素を全て交換する */
	void swap(intrusive_list &l) {base_class::swap(l);}

//!	マージ
/*!
	otherの全要素を取り入れる。otherは空になる。\n
	自身、other共にlessでソートされていればlessでソートした順列で格納される。\n
	そうでなければ格納される順列は不定。
	@param other マージして空になるリスト
	@param less 要素比較述語関数オブジェクト
*/
	template <class BinaryPredicate>
	void merge(intrusive_list &other,BinaryPredicate less)
		{intrusive_list_merge(*this,other,less);}

//!	マージソート
/*!
	要素をlessで比較した結果によって並び替える。
	@pre assert(size() <= (2^16 * 2))\n
		実際にはsize()を呼ぶわけではなく、
		処理した数が限界に達した時点でassert(false)となる。
	@param less 要素比較述語関数オブジェクト
*/
	template <class BinaryPredicate>
	void sort(BinaryPredicate less)
		{intrusive_list_sort<16>(*this,less);}

//	これだけのために#include <functional>も何なので
	struct less
	{bool operator()(const_reference lhs,const_reference rhs) const {return lhs < rhs;}};

//	しかし、なんかこいつらPredicateを直接指定するより遅い。どっちにしてもVC6（略

//!	マージ
/*!
	otherの全要素を取り入れる。otherは空になる。\n
	自身、other共にソートされていればソートした順列で格納される。\n
	そうでなければ格納される順列は不定。
	@param other マージして空になるリスト
*/
	void merge(intrusive_list &other) {merge(other,less());}
//!	マージソート
/*!
	要素を<演算子で比較した結果によって並び替える。
	@pre assert(size() <= (2^16 * 2))\n
		実際にはsize()を呼ぶわけではなく、
		処理した数が限界に達した時点でassert(false)となる。
*/
	void sort(void) {sort(less());}


//!	要素を逆順に並べ替える
	void reverse(void)
	{
		for (iterator i = begin();i != --end();splice(i,--end()))
			;
	}

//!	一致する要素を排除する
/*!	x == val がtrueとなる要素xをerase()する。 */
	void remove(const_reference val)
	{
		for (iterator it = begin();it != end();)
			if (*it == val)
				it = erase(it);
			else
				++it;
	}
//!	条件に一致する要素を排除する
/*!	test(x) がtrueとなる要素xをerase()する。 */
	template <class UnaryPredicate>
	void remove_if(UnaryPredicate test)
	{
		for (iterator it = begin();it != end();)
			if (test(*it))
				it = erase(it);
			else
				++it;
	}
//@}

//	void unique(void);
//	template <class BinaryPredicate>
//	void unique(BinaryPredicate equal);


//operator==()とか使うか？


//! @name ソート済み操作
//@{
//!	ソート済み挿入
/*!
	lessでソートされていれば、insもソートされた位置に挿入される。\n
	そうでなければ挿入される位置は不定。
	@return 挿入した要素を指すイテレータ
	@pre assert(ins.linked() == false)
*/
	template <class BinaryPredicate>
	iterator insert(reference ins,BinaryPredicate less)
	{
		for (iterator i = begin();i != end();++i)
			if (less(ins,*i))
				return insert(i,ins);
		return insert(end(),ins);
	}
//!	ソート済み繋ぎ替え
/*!
	lessでソートされていれば、iもソートされた位置に繋ぎ替えられる。\n
	そうでなければ繋ぎ替えられる位置は不定。
	@pre assert(ins.linked() == false)
	@post iは繋ぎ替えられた後の要素を指している。
*/
	template <class BinaryPredicate>
	void splice(iterator i,BinaryPredicate less)
	{
		for (iterator it = begin();it != end();++it)
			if (less(*i,*it))
				{splice(it,i);return;}
		splice(end(),i);
	}
//!	ソート済み挿入
/*!
	ソートされていれば、insもソートされた位置に挿入される。\n
	そうでなければ挿入される位置は不定。
	@return 挿入した要素を指すイテレータ
	@pre assert(ins.linked() == false)
*/
	iterator insert(reference ins) {return insert(ins,less());}
//!	ソート済み繋ぎ替え
/*!
	ソートされていれば、iもソートされた位置に繋ぎ替えられる。\n
	そうでなければ繋ぎ替えられる位置は不定。
	@pre assert(ins.linked() == false)
	@post iは繋ぎ替えられた後の要素を指している。
*/
	void splice(iterator i) {return splice(i,less());}
//@}


//!	実験コード
/*!	要素の型を変えた参照を得る */
	template<typename U,int TagNumber>
	intrusive_list<U,TagNumber> &static_cast_element_list(void)
	{
		return static_cast<intrusive_list<node_type,TagNumber> &>(static_cast<base_intrusive_list_<TagNumber> &>(*this));
		static_cast<U *>(static_cast<node_type *>(0));
	}
	template<typename U,int TagNumber>
	intrusive_list<U,TagNumber> &static_cast_element_list(void) const
	{
		return static_cast<const intrusive_list<U,TagNumber> &>(static_cast<const base_intrusive_list_<TagNumber> &>(*this));
		static_cast<U *>(static_cast<node_type *>(0));
	}
	intrusive_list<const node_type,TagNumber> &const_element_list(void)
	{
		return static_cast<intrusive_list<const node_type,TagNumber> &>(static_cast<base_intrusive_list_<TagNumber> &>(*this));
	}
	const intrusive_list<const node_type,TagNumber> &const_element_list(void) const
	{
		return static_cast<const intrusive_list<const node_type,TagNumber> &>(static_cast<const base_intrusive_list_<TagNumber> &>(*this));
	}
};
//template<class DerivedNode,int TagNumber> class intrusive_list



//!	リスト間の要素全交換
/*!	l1とl2の要素を全て交換する */
template<class T,int N>
void swap(intrusive_list<T,N> &l1,intrusive_list<T,N> &l2) {l1.swap(l2);}

/*!
	otherの全要素をlistに取り入れる。otherは空になる。\n
	list,other共にlessでソートされていればlessでソートした順列で格納される。\n
	そうでなければ格納される順列は不定。
	@param list マージして結果を格納するリスト
	@param other マージして空になるリスト
	@param less 要素比較述語関数オブジェクト
*/
template <class IntrusiveListT,class BinaryPredicate>
void intrusive_list_merge(IntrusiveListT &list,IntrusiveListT &other,BinaryPredicate less)
{
	assert(&list != &other);
	if (list.empty() || other.empty())
	{
		list.splice(list.end(),other);
		return;
	}
	for (IntrusiveListT::iterator pos = list.begin();;)
	{
		if (less(other.front(),*pos))
		{
			list.splice(pos,other.begin());
			if (other.empty())
				break;
		}
		else
		{
			if (++pos == list.end())
			{
				list.splice(list.end(),other);
				break;
			}
		}
	}
}

/*!
	要素をlessで比較した結果によって並び替える。\n
	テンプレート引数でソート用スタックサイズを指定できる
	@pre assert(size() <= (2^sort_stack_size * 2))\n
		実際にはsize()を呼ぶわけではなく、
		処理した数が限界に達した時点でassert(false)となる。

	@param sort_stack_size スタックサイズ

	@param list ソートするリスト
	@param less 要素比較述語関数オブジェクト
*/
template <int sort_stack_size,class IntrusiveListT,class BinaryPredicate>
void intrusive_list_sort(IntrusiveListT &list,BinaryPredicate less)
{
	IntrusiveListT stack[sort_stack_size];	//作業領域
	int m_count[sort_stack_size] = {0};		//マージ回数

	int n = 0;
	while (!list.empty())
	{
		list.splice(stack[n].end(),list.begin());
		if (!list.empty())
		{
			if (less(list.front(),stack[n].front()))
				list.splice(stack[n].begin(),list.begin());
			else
				list.splice(stack[n].end(),list.begin());
		}
		m_count[n] = 1;

		int i;
		for (i = n;i > 0;i--)
		{
			if (m_count[i] != m_count[i-1])
				break;
			stack[i-1].merge(stack[i],less);
			m_count[i-1] += m_count[i];
		}
		n = i+1;
//	スタックオーバーフローすると死
		assert(n < sort_stack_size);
	}
//	for (int i = 0;i < n;i++)
//		merge(stack[i],less);
	for (int i = n-1;i > 0;i--)
		stack[i-1].merge(stack[i],less);
	list.splice(list.end(),stack[0]);
}


}//namespace Juna


#endif
