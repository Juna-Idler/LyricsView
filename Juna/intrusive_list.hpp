
/*!
	@file
	@brief �N���^�o�����A�����X�g�R���e�i

	����
	- �R���e�i������ɗv�f�����X�g����O�����Ƃ��ł���B
	- �v�f�̓R�s�[�\�łȂ��Ă��\��Ȃ��B
	- �����Ń������m�ۑ�������Ȃ��B
	- �|�C���^�̃��X�g��菭�����������������������B

	- intrusive_list_node���p�����Ȃ���΂Ȃ�Ȃ��B
	- �v�f���͕̂ʂ̈��肵���ꏊ�Ŋm�ۂ��Ȃ���΂Ȃ�Ȃ��B

	@author Juna
*/

#ifndef Juna_Intrusive_Linked_List_Hpp
#define Juna_Intrusive_Linked_List_Hpp


#include <stddef.h>
#include <iterator>
#include <assert.h>


//!	namespace Juna
namespace Juna {

//!	���X�g�v�f�̐N�������̊�b����
/*!
*/
class base_intrusive_list_node
{
protected:
//!	�f�t�H���g�R���X�g���N�^
/*!
	@post linked() == false
*/
	base_intrusive_list_node(void) {next = prev = this;}
//!	�f�X�g���N�^
/*!
	@pre assert(linked() == false)
*/
	~base_intrusive_list_node()
	{
		assert(!linked());	//�q�������܂܏������
	}

	//
//!	���X�g�Ɍq�����Ă��邩
/*!
	@retval true �q�����Ă���
	@retval false �q�����Ă��Ȃ�
*/
	bool linked(void) const {return next != this;}

//!	���X�g����O���
/*!
	���X�g������Ƀ��X�g����O���B
*/
	void unlink(void)
	{
		prev->next = next;
		next->prev = prev;
		next = prev = this;
	}
private:
//	this->next == &other�ƂȂ�悤�Ɍq����B
/*	��{�I�ɂǂ��炩�����ڑ��̏�ԂŎg�p����B */
	void link_next(base_intrusive_list_node &other)
	{
		other.prev->next = next;
		next->prev = other.prev;
		other.prev = this;
		next = &other;
	}
//	����ւ�
/*	other�ƃ����N�i�ʒu�j�����ւ���B */
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

//!	���X�g�v�f�̐N������
/*!
	������p�����邱�ƂŃ��X�g�̗v�f�ƂȂ邱�Ƃ��ł���B
	�e���v���[�g�����́A�v�f�������̃��X�g�ɓo�^�����ꍇ�ɗ��p����B
*/
template<int TagNumber>
class intrusive_list_node_ : public base_intrusive_list_node {};

//!	���X�g�v�f�̐N������
/*!
	�P��̗v�f�̂��߂̕ʖ�
*/
typedef intrusive_list_node_<0> intrusive_list_node;

/*===========================================================================*/


//	�N���^�o�������X�g�R���e�i�N���X�̋��ʕ���
//	base_intrusive_list_node��friend
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

//	base_intrusive_list����

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

//	�N���^�o�������X�g�R���e�i�N���X�̃^�O�̐U�蕪������
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

//!	�}�[�W
template <class IntrusiveListT,class BinaryPredicate>
void intrusive_list_merge(IntrusiveListT &list,IntrusiveListT &other,BinaryPredicate less);
//!	�}�[�W�\�[�g
template <int sort_stack_size,class IntrusiveListT,class BinaryPredicate>
void intrusive_list_sort(IntrusiveListT &list,BinaryPredicate less);


/*===========================================================================*/

//!	�N���^�o�������X�g�R���e�i�N���X�e���v���[�g
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

//	���C�e���[�^�p�|���V�[
	struct normal_step
	{
		template<class T> static T *inc(T *node) {return next(node);}
		template<class T> static T *dec(T *node) {return prev(node);}
	};
//	�t�C�e���[�^�p�|���V�[
	struct reverse_step
	{
		template<class T> static T *inc(T *node) {return prev(node);}
		template<class T> static T *dec(T *node) {return next(node);}
	};
//	node�|�C���^���쌠��^����
	friend struct normal_step;
	friend struct reverse_step;

//	�C�e���[�^�̎���
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
//	�֗��֐�
	static pointer next(const ilist_node *node) {return static_cast<pointer>(base_class::next(node));}
	static pointer prev(const ilist_node *node) {return static_cast<pointer>(base_class::prev(node));}
	static pointer cast(ilist_node *node) {return static_cast<pointer>(node);}
	static pointer next(const_pointer node) {return static_cast<pointer>(base_class::next(node));}
	static pointer prev(const_pointer node) {return static_cast<pointer>(base_class::prev(node));}

	ilist_node &header(void) const {return static_cast<ilist_node &>(static_cast<base_intrusive_list_node &>(base_class::header));}

public:
//!	�C�e���[�^ (bidirectional_iterator)
	typedef base_iterator<normal_step> iterator;
//!	�R���X�g�C�e���[�^ (bidirectional_iterator)
	typedef base_const_iterator<normal_step> const_iterator;
//!	���o�[�X�C�e���[�^
	typedef base_iterator<reverse_step> reverse_iterator;
//!	�R���X�g���o�[�X�C�e���[�^
	typedef base_const_iterator<reverse_step> const_reverse_iterator;

//!	�f�t�H���g�R���X�g���N�^
/*!	����ȊO�ɃR���X�g���N�^�͖��� */
	intrusive_list(void) : base_intrusive_list_<TagNumber>() {}
//!	�f�X�g���N�^
/*!	clear()���Ă� */
	~intrusive_list() {clear();}

//!	@name �S�v�f�r��
//@{
//!	�S�v�f�r��
/*!	���̃��X�g�̑S�v�f�����X�g����r������B */
	void clear(void) {base_class::clear();}
//@}

//!	@name �v�f��
//@{
//!	�v�f��
/*!	�����������X�g�����ǂ��Đ����܂��B */
	size_type size(void) const {return base_class::size();}
//!	���X�g���󂩂ǂ���
/*!	size()��葬�� size() == 0 */
	bool empty(void) const {return base_class::empty();}
//@}

//!	@name �[�̗v�f�Q��
//@{
//!	�擪�v�f�Q��
/*! @pre empty() == false */
	reference front(void) {return *next(&header());}
//!	�擪�v�f�R���X�g�Q��
/*! @pre empty() == false */
	reference front(void) const {return *next(&header());}

//!	�����v�f�Q��
/*! @pre empty() == false */
	reference back(void) {return *prev(&header());}
//!	�����v�f�R���X�g�Q��
/*! @pre empty() == false */
	reference back(void) const {return *prev(&header());}
//@}

//!	@name �X�^�b�N����
//@{
//!	�擪�ւ�push
/*!
	���X�g�̐擪��ins��}������B
	@pre assert(ins.linked() == false)
*/
	void push_front(reference ins) {base_class::insert_after(&header(),&ins);}
//!	�����ւ�push
/*!
	���X�g�̖�����ins��}������B
	@pre assert(ins.linked() == false)
*/
	void push_back(reference ins) {base_class::insert(&header(),&ins);}

//!	�擪�v�f��pop
/*!
	�擪�v�f��r������B
	@pre empty() == false
*/
	void pop_front(void) {base_class::erase(next(&header()));}
//!	�����v�f��pop
/*!
	�����v�f��r������B
	@pre empty() == false
*/
	void pop_back(void) {base_class::erase(prev(&header()));}
//@}

//!	@name �C�e���[�^
//@{
//!	�擪�C�e���[�^
	iterator begin(void) {return next(&header());}
//!	�R���X�g�擪�C�e���[�^
	const_iterator begin(void) const {return next(&header());}
//!	�I�[�C�e���[�^
	iterator end(void) {return cast(&header());}
//!	�R���X�g�I�[�C�e���[�^
	const_iterator end(void) const {return cast(&header());}
//!	�t�擪�C�e���[�^
	reverse_iterator rbegin(void) {return prev(&header());}
//!	�R���X�g�t�擪�C�e���[�^
	const_reverse_iterator rbegin(void) const {return prev(&header());}
//!	�t�I�[�C�e���[�^
	reverse_iterator rend(void) {return cast(&header());}
//!	�R���X�g�t�I�[�C�e���[�^
	const_reverse_iterator rend(void) const {return cast(&header());}
//@}


//	�}���A�r���A�q���ւ�
//	�ʒu�w��C�e���[�^�ɂ����ˑ����Ȃ�����A�ςȊ��������邪�S��static
//	�����悭����Ȃ����ǃC�e���[�^���Q�ƂŎ󂯂Ȃ��Ɓufatal error C1001�v���N���邱�Ƃ�
//	�ǂ����ɂ��Ă�VC6���낤���ǁB

//!	@name �}��
//@{
//!	�v�f�̑}��
/*!
	pos�̈ʒu��ins��}������B
	@return �}�������v�f���w���C�e���[�^
	@pre assert(ins.linked() == false)
*/
	iterator insert(iterator pos,reference ins)
		{return cast(base_class::insert(pos.get(),&ins));}
//!	�v�f�̑}��
/*!
	pos�̎��̈ʒu��ins��}������B
	@return �}�������v�f���w���C�e���[�^
	@pre assert(ins.linked() == false)
*/
	iterator insert_after(iterator pos,reference ins)
		{return cast(base_class::insert_after(pos.get(),&ins));}
//@}

//!	@name �r��
//@{
//!	�v�f�̔r��
/*!
	pos�̗v�f�����X�g����r������B
	@return �폜�����v�f�̎��̗v�f���w���C�e���[�^
	@post pos�̗v�f�ւ̎Q�Ƃ͗L���B
*/
	iterator erase(iterator pos)
		{return cast(base_class::erase(pos.get()));}
//!	�v�f�̔r��
/*!
	pos�̎��̗v�f�����X�g����r������B
	@return �폜�����v�f�̎��̗v�f���w���C�e���[�^
*/
	iterator erase_after(iterator pos)
		{return cast(base_class::erase(next(pos.get())));}
//@}

//!	@name �v�f�̌q���ւ�
//@{
//!	�v�f�̌q���ւ�
/*!
	l�̑S�v�f��pos�̈ʒu�Ɍq���ւ���Bl�͋�ɂȂ�B
*/
	void splice(iterator pos,intrusive_list &l)
		{base_class::splice(pos.get(),l.header());}
//!	�v�f�̌q���ւ�
/*!
	i�̗v�f��pos�̈ʒu�Ɍq���ւ���B
	@pre assert(pos != i)
*/
	void splice(iterator pos,intrusive_list &,iterator i)
		{splice(pos,i);}
//!	�v�f�̌q���ւ�
/*!
	[first-last)�͈̔͂ɂ���v�f��pos�̈ʒu�Ɍq���ւ���B
	@pre assert(first != last)
	@pre first-last�Ԃ�pos�������Ă͂Ȃ�Ȃ��B
*/
	void splice(iterator pos,intrusive_list &,iterator first,iterator last)
		{splice(pos,first,last);}
//@}

//!	@name �v�f�̌q���ւ�
//@{
//!	�v�f�̌q���ւ�
/*!
	i�̗v�f��pos�̈ʒu�Ɍq���ւ���B
	@pre assert(pos != i)
	�݊��p�̂���Ȃ�������������
*/
	static void splice(iterator pos,iterator i)
		{base_class::splice(pos.get(),i.get());}
//!	�v�f�̌q���ւ�
/*!
	[first-last)�͈̔͂ɂ���v�f��pos�̈ʒu�Ɍq���ւ���B
	@pre assert(first != last)
	@pre first-last�Ԃ�pos�������Ă͂Ȃ�Ȃ��B
	�݊��p�̂���Ȃ�������������
*/
	static void splice(iterator pos,iterator first,iterator last)
		{base_class::splice(pos.get(),first.get(),last.get());}


//!	@name �A���S���Y��
//@{
//!	���X�g�Ԃ̗v�f�̑S����
/*!	l�Ɨv�f��S�Č������� */
	void swap(intrusive_list &l) {base_class::swap(l);}

//!	�}�[�W
/*!
	other�̑S�v�f���������Bother�͋�ɂȂ�B\n
	���g�Aother����less�Ń\�[�g����Ă����less�Ń\�[�g��������Ŋi�[�����B\n
	�����łȂ���Ίi�[����鏇��͕s��B
	@param other �}�[�W���ċ�ɂȂ郊�X�g
	@param less �v�f��r�q��֐��I�u�W�F�N�g
*/
	template <class BinaryPredicate>
	void merge(intrusive_list &other,BinaryPredicate less)
		{intrusive_list_merge(*this,other,less);}

//!	�}�[�W�\�[�g
/*!
	�v�f��less�Ŕ�r�������ʂɂ���ĕ��ёւ���B
	@pre assert(size() <= (2^16 * 2))\n
		���ۂɂ�size()���ĂԂ킯�ł͂Ȃ��A
		���������������E�ɒB�������_��assert(false)�ƂȂ�B
	@param less �v�f��r�q��֐��I�u�W�F�N�g
*/
	template <class BinaryPredicate>
	void sort(BinaryPredicate less)
		{intrusive_list_sort<16>(*this,less);}

//	���ꂾ���̂��߂�#include <functional>�����Ȃ̂�
	struct less
	{bool operator()(const_reference lhs,const_reference rhs) const {return lhs < rhs;}};

//	�������A�Ȃ񂩂�����Predicate�𒼐ڎw�肷����x���B�ǂ����ɂ��Ă�VC6�i��

//!	�}�[�W
/*!
	other�̑S�v�f���������Bother�͋�ɂȂ�B\n
	���g�Aother���Ƀ\�[�g����Ă���΃\�[�g��������Ŋi�[�����B\n
	�����łȂ���Ίi�[����鏇��͕s��B
	@param other �}�[�W���ċ�ɂȂ郊�X�g
*/
	void merge(intrusive_list &other) {merge(other,less());}
//!	�}�[�W�\�[�g
/*!
	�v�f��<���Z�q�Ŕ�r�������ʂɂ���ĕ��ёւ���B
	@pre assert(size() <= (2^16 * 2))\n
		���ۂɂ�size()���ĂԂ킯�ł͂Ȃ��A
		���������������E�ɒB�������_��assert(false)�ƂȂ�B
*/
	void sort(void) {sort(less());}


//!	�v�f���t���ɕ��בւ���
	void reverse(void)
	{
		for (iterator i = begin();i != --end();splice(i,--end()))
			;
	}

//!	��v����v�f��r������
/*!	x == val ��true�ƂȂ�v�fx��erase()����B */
	void remove(const_reference val)
	{
		for (iterator it = begin();it != end();)
			if (*it == val)
				it = erase(it);
			else
				++it;
	}
//!	�����Ɉ�v����v�f��r������
/*!	test(x) ��true�ƂȂ�v�fx��erase()����B */
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


//operator==()�Ƃ��g�����H


//! @name �\�[�g�ςݑ���
//@{
//!	�\�[�g�ςݑ}��
/*!
	less�Ń\�[�g����Ă���΁Ains���\�[�g���ꂽ�ʒu�ɑ}�������B\n
	�����łȂ���Α}�������ʒu�͕s��B
	@return �}�������v�f���w���C�e���[�^
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
//!	�\�[�g�ς݌q���ւ�
/*!
	less�Ń\�[�g����Ă���΁Ai���\�[�g���ꂽ�ʒu�Ɍq���ւ�����B\n
	�����łȂ���Όq���ւ�����ʒu�͕s��B
	@pre assert(ins.linked() == false)
	@post i�͌q���ւ���ꂽ��̗v�f���w���Ă���B
*/
	template <class BinaryPredicate>
	void splice(iterator i,BinaryPredicate less)
	{
		for (iterator it = begin();it != end();++it)
			if (less(*i,*it))
				{splice(it,i);return;}
		splice(end(),i);
	}
//!	�\�[�g�ςݑ}��
/*!
	�\�[�g����Ă���΁Ains���\�[�g���ꂽ�ʒu�ɑ}�������B\n
	�����łȂ���Α}�������ʒu�͕s��B
	@return �}�������v�f���w���C�e���[�^
	@pre assert(ins.linked() == false)
*/
	iterator insert(reference ins) {return insert(ins,less());}
//!	�\�[�g�ς݌q���ւ�
/*!
	�\�[�g����Ă���΁Ai���\�[�g���ꂽ�ʒu�Ɍq���ւ�����B\n
	�����łȂ���Όq���ւ�����ʒu�͕s��B
	@pre assert(ins.linked() == false)
	@post i�͌q���ւ���ꂽ��̗v�f���w���Ă���B
*/
	void splice(iterator i) {return splice(i,less());}
//@}


//!	�����R�[�h
/*!	�v�f�̌^��ς����Q�Ƃ𓾂� */
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



//!	���X�g�Ԃ̗v�f�S����
/*!	l1��l2�̗v�f��S�Č������� */
template<class T,int N>
void swap(intrusive_list<T,N> &l1,intrusive_list<T,N> &l2) {l1.swap(l2);}

/*!
	other�̑S�v�f��list�Ɏ������Bother�͋�ɂȂ�B\n
	list,other����less�Ń\�[�g����Ă����less�Ń\�[�g��������Ŋi�[�����B\n
	�����łȂ���Ίi�[����鏇��͕s��B
	@param list �}�[�W���Č��ʂ��i�[���郊�X�g
	@param other �}�[�W���ċ�ɂȂ郊�X�g
	@param less �v�f��r�q��֐��I�u�W�F�N�g
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
	�v�f��less�Ŕ�r�������ʂɂ���ĕ��ёւ���B\n
	�e���v���[�g�����Ń\�[�g�p�X�^�b�N�T�C�Y���w��ł���
	@pre assert(size() <= (2^sort_stack_size * 2))\n
		���ۂɂ�size()���ĂԂ킯�ł͂Ȃ��A
		���������������E�ɒB�������_��assert(false)�ƂȂ�B

	@param sort_stack_size �X�^�b�N�T�C�Y

	@param list �\�[�g���郊�X�g
	@param less �v�f��r�q��֐��I�u�W�F�N�g
*/
template <int sort_stack_size,class IntrusiveListT,class BinaryPredicate>
void intrusive_list_sort(IntrusiveListT &list,BinaryPredicate less)
{
	IntrusiveListT stack[sort_stack_size];	//��Ɨ̈�
	int m_count[sort_stack_size] = {0};		//�}�[�W��

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
//	�X�^�b�N�I�[�o�[�t���[����Ǝ�
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
