#pragma once
#include <string>
#include "Error.h"
#include <iostream>

template <typename T> class ARRAY {

	T* ptr;
	unsigned int length;

public:

	ARRAY(void) : ptr(), length() {}

	ARRAY(unsigned int length) : ptr(nullptr), length(length) {
		if (this->length) ptr = new T[this->length]();	
	}

	~ARRAY(void) {	
		delete[] ptr;
	}

	T& operator [](unsigned int n) {

		if (n < length) return ptr[n];
		else {
			error::tARRAY::elementexceed(derived());
			return *(new T);
		}

	}	

	const T& operator [](unsigned int n) const {

		if (n < length)	return ptr[n];
		else {
			error::tARRAY::elementexceed(derived());
			return *(new T);
		}

	}

	ARRAY<T>& operator <<(ARRAY<T>& move) {

		ARRAY<T> temp; 

		temp.ptr = this->ptr;
		temp.length = this->length;
		this->ptr = move.ptr;
		this->length = move.length;
		move.ptr = nullptr;
		move.length = 0;

		return *this;

	}

	ARRAY<T>& operator <<(ARRAY<T>&& move) {	
		return (*this << move);
	}

private:

	ARRAY<T>& operator =(ARRAY<T>& move) {	
		return (*this << move);
	}

public:

	bool operator ==(const ARRAY<T>& arr) const {
		return (ptr == arr.ptr) && (length == arr.length);
	}

	bool operator !=(const ARRAY<T>& arr) const {
		return !((*this) == arr);
	}

	unsigned int getlength(void) const { return length; }

	void ReAllocate(unsigned int length) {

		ARRAY<T> temp(length);

		unsigned int N(std::min(length, this->length));

		for (unsigned int i(0); i < N; ++i) {	
			temp[i] = ptr[i];
		}

		(*this) << temp;

	}

	void show(void) const {
		std::cout << "show ARRAY:[";
		for (unsigned int n(0); n < getlength(); ++n) { std::cout << (*this)[n] << " "; }
		std::cout << "]\n";
	}

private:

	ARRAY(const ARRAY<T>& in);	
	virtual std::string derived(void) const { return ""; }

};

/*

	・std;;vectorのような、配列を扱うクラス。

	・std::vectorのようなemplace_back機能はない。

	・コピー禁止にしている。

		・が、<<演算子を用いて「移譲」できる。
			(A << BとすればBのメンバをAに移譲し、元々AにあったものはdeleteしてBは初期化している。)


	・std::vectorの速さが気になる場合などに。

*/

/***************************************************************************

	StdBoard

****************************************************************************/

/*
 
	・要素数の固定された配列に要素の登録を扱うために用意したクラス。

	・ARRAY<Template>を継承。

	・Template()となっている要素は「空」を意味するとする。

	・「占有数」を数える。

*/

template <typename Template>
class StdBoard : public ARRAY<Template> {

	using succeed = bool;

public:

	StdBoard(void) : currentpoint(-1), occupiedlength() {}

	StdBoard(unsigned int length) : ARRAY<Template>(length), currentpoint(-1), occupiedlength() {}

	bool operator ==(const StdBoard<Template>& stdboard) const {
		return ((ARRAY<Template>&)(*this) == (ARRAY<Template>&)stdboard) && (currentpoint == stdboard.currentpoint) && (occupiedlength == stdboard.occupiedlength);
	}

	bool operator !=(const StdBoard<Template>& stdboard) const { return !((*this) == stdboard); }

	StdBoard<Template>& operator <<(StdBoard<Template>& move) {	

		(ARRAY<Template>&)(*this) << (ARRAY<Template>&)move;

		currentpoint = move.currentpoint;
		occupiedlength = move.occupiedlength;
		move.currentpoint = -1; 
		move.occupiedlength = 0;
		return *this;

	}

	StdBoard<Template>& operator <<(StdBoard<Template>&& move) { return (*this << move); }

	const Template& operator [](unsigned int n) { return ((ARRAY<Template>&)(*this))[n]; }

	const Template& operator [](unsigned int n) const { return ((ARRAY<Template>&)(*this))[n]; }

	void ReAllocate(unsigned int length) {

		((ARRAY<Template>&)(*this)).ReAllocate(length);

		unsigned int occupied(0);

		for (unsigned int n(0); n < this->getlength(); ++n) {
			if ((*this)[n] != Template()) ++occupied;
		}

		occupiedlength = occupied;
		currentpoint = -1;	

	}

	succeed Register(const Template& instance) {

		succeed registered(false);

		if (instance != Template()) {

			if (occupiedlength != this->getlength()) { 

				ARRAY<Template>& tarray((ARRAY<Template>&)(*this));
				while (tarray[(++currentpoint) %= this->getlength()] != Template())
					;
				tarray[currentpoint] = instance;
				++occupiedlength;
				registered = true;

			}

		}

		else error::tBoard::Nullregister();

		return registered;

	}

	void Remove(unsigned int num) {

		Template& value(((ARRAY<Template>&)(*this))[num]);

		if (value != Template()) {
			value = Template();
			--occupiedlength;
		}

		else error::tBoard::Nullremove();

	}
	unsigned int getcurrentpoint(void) { return currentpoint; } 

	bool isempty(void) {
		bool empty(false);
		if (!occupiedlength) empty = true;
		return empty;
	}	
	void show() {
		std::cout << "currentpoint : " << currentpoint << " occupiedlength : " << occupiedlength << "\n[ ";
		for (unsigned int i(0); i < this->getlength(); ++i) {
			std::cout << (*this)[i] << " ";
		}
		std::cout << "]\n";
	}

private:

	unsigned int currentpoint;
	unsigned int occupiedlength;

	virtual std::string derived(void) { return "(StdBoard)"; }

};

/***************************************************************************

	Board

****************************************************************************/

/*

	・StdBoard<Template*>を継承。

	・要素はptrなのでdeleteが扱える。

*/

template <typename Template>
class Board : public StdBoard<Template*> {

public:

	Board(void) {}

	Board(unsigned int length) : StdBoard<Template*>(length) {};

	~Board(void) {

		for (unsigned int n(0); n < this->getlength(); ++n) {
			delete (*this)[n];
		}

	}

	bool operator ==(const Board<Template*>& board) const {
		return ((StdBoard<Template*>&)(*this) == (StdBoard<Template*>&)board);
	}

	bool operator !=(const Board<Template*>& board) const { return !((*this) == board); }

	void Remove(unsigned int num) {

		delete (*this)[num];
		((StdBoard<Template*>&)(*this)).Remove(num);

	}


	void ReAllocate(unsigned int length) {

		if (length >= this->getlength()) ((StdBoard<Template*>&)(*this)).ReAllocate(length);
		else error::tBoard::shortenreallocate();

	}

private:

	Board<Template*>& operator <<(Board<Template*>& move);
	Board<Template*>& operator <<(Board<Template*>&& move);

	virtual std::string derived(void) { return "(Board)"; }
};