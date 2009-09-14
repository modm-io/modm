// coding: utf-8
// ----------------------------------------------------------------------------
/**
 * \defgroup	arithmetic_trais	Arithmetic Traits
 * \brief		Traits für aritmetische (Zahlen) Typen
 * 
 * Traits bieten elegante Möglichkeit informationen an einen Typen anzuhängen 
 * ohne die Implementation dieses Typs zu ändern.
 * 
 * \section Elemente des Traits
 * 
 * \type	double_type	Kann das Ergenis einer Multiplication des Typs T
 * 						aufnehmen
 * \value	min_value	kleinster darstellbarer Wert (int: Negative, 
 * 						float/double: kleinste pos Zahl)
 * \value	max_value	größte darstellbarer Wert
 * \value	signed_type	TODO
 * \value	unsigned_type	TODO
 * 
 * \section Benutzen der Klasse
 * 
 * \subsection	Einen Typ (T_DOUBLE) definieren, in den das Ergebnis 
 * 				von (T * T) passt
 * \code
 * typedef typename arithmetic_traits<T>::double_type T_DOUBLE;
 * \endcode
 * 
 * \subsection min/max Wert des Typ erhalten
 * \code
 * T min = arithmetic_traits<T>::min_value;
 * T max = arithmetic_traits<T>::max_value;
 * \endcode
 *
 * \version	$Id: arithmetic_traits.h 8106 2009-08-17 14:57:59Z fabian $
 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
// ----------------------------------------------------------------------------
/*@{*/

#ifndef ARITHMETIC_TRAITS_H
#define ARITHMETIC_TRAITS_H

#include <stdint.h>

// Nutzen des Default-Templates verursacht einen Compiler-Fehler 
// (gewollt, besser als Laufzeit-Fehler!)
template<typename T>
struct arithmetic_traits {
	
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<int8_t> {
	typedef int16_t double_type;
	typedef int8_t signed_type;
	typedef uint8_t unsigned_type;
	
	static const int8_t min_value = -0x80;
	static const int8_t max_value =  0x7F;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<uint8_t> {
	typedef uint16_t double_type;
	typedef int8_t signed_type;
	typedef uint8_t unsigned_type;
	
	static const uint8_t min_value = 0;
	static const uint8_t max_value = 0xff;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<int16_t> {
	typedef int32_t double_type;
	typedef int16_t signed_type;
	typedef uint16_t unsigned_type;
	
	static const int16_t min_value = -0x8000;
	static const int16_t max_value =  0x7FFF;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<uint16_t> {
	typedef uint32_t double_type;
	typedef int16_t signed_type;
	typedef uint16_t unsigned_type;
	
	static const uint16_t min_value = 0;
	static const uint16_t max_value = 0xFFFF;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<int32_t> {
	typedef float double_type; // int64_t ist bei AVRs auch nur ein int32_t
	typedef int32_t signed_type;
	typedef uint32_t unsigned_type;
	
	static const int32_t min_value = -0x80000000;
	static const int32_t max_value =  0x7FFFFFFF;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<uint32_t> {
	typedef float double_type; // int64_t ist bei AVRs auch nur ein int32_t
	typedef int32_t signed_type;
	typedef uint32_t unsigned_type;
	
	static const uint32_t min_value = 0;
	static const uint32_t max_value = 0xFFFFFFFF;
};

// ----------------------------------------------------------------------------
template<>
struct arithmetic_traits<float> {
	typedef float double_type;
	typedef float signed_type;
	typedef float unsigned_type;
	
	static const float min_value = 1.17549e-38;
	static const float max_value = 3.40282e+38;
};

/*@}*/
#endif	// ARITHMETIC_TRAITS_H
