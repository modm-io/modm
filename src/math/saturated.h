// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \file		saturated.h
 * \brief		saturierter Arithmetik
 * 
 * \author		Fabian Greif <fabian.greif@rwth-aachen.de>
 * \version		$Id: saturated.h 8106 2009-08-17 14:57:59Z fabian $
 */
// ----------------------------------------------------------------------------

#ifndef	SATURATED_H
#define	SATURATED_H

// TODO
#include "arithmetic_traits.h"

// see http://www.mikrocontroller.net/articles/AVR_Arithmetik/Saturierung
template<typename T>
class Saturated
{
	public:
		typedef typename arithmetic_traits<T>::signed_type signed_type;
		typedef typename arithmetic_traits<T>::unsigned_type unsigned_type;
		
		friend class Saturated<signed_type>;
		friend class Saturated<unsigned_type>;
		
		template<typename U> friend Saturated<U> operator-(const Saturated<U>& x);
		template<typename U> friend Saturated<U> abs(const Saturated<U>& x);
	
	public:
		Saturated(T x=0) : m_value(x) {
			// nothing else to do here
		}
		
		T
		value() {
			return m_value;
		}
		
		Saturated&
		operator+=(const Saturated<signed_type>& other);
		
		Saturated&
		operator+=(const Saturated<unsigned_type>& other);
		
		Saturated&
		operator-=(const Saturated<signed_type>& other);
		
		Saturated&
		operator-=(const Saturated<unsigned_type>& other);
	
	private:
		T m_value;
};

// ----------------------------------------------------------------------------
// 							GLOBAL FUNCTIONS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Saturated<T>
operator-(const Saturated<T>& x);

// ----------------------------------------------------------------------------
// WARNING: this implementation is only correct for unsigned-types,
// the functions needs to be specialized for all the signed-types
template<typename T>
Saturated<T>
abs(const Saturated<T>& x) {
	return Saturated<T>(x);
}

// ----------------------------------------------------------------------------
template<typename T>
Saturated<T>
operator-(const Saturated<T>& a, const Saturated<T>& b) {
	Saturated<T> t(a);
	t -= b;
	return t;
}

// ----------------------------------------------------------------------------
template<typename T>
Saturated<T>
operator+(const Saturated<T>& a, const Saturated<T>& b) {
	Saturated<T> t(a);
	t += b;
	return t;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator==(const Saturated<T>& a, const Saturated<T>& b) {
	return (a.value() == b.value());
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator!=(const Saturated<T>& a, const Saturated<T>& b) {
	return (a.value() != b.value());
}

#ifdef __AVR__
	#include "saturated/saturated_impl.hpp"
#endif

#endif	// SATURATED_H
