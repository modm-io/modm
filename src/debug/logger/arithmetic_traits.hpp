#ifndef ARITHMETIC_TRAITS_H
#define ARITHMETIC_TRAITS_H

/**
 * \defgroup arithmetic_trais	Arithmetic Traits
 * \brief	Traits f�r aritmetische (Zahlen) Typen
 * 
 * Traits bieten elegante M�glichkeit informationen an einen Typen anzuh�ngen ohne
 * die Implementation dieses Typs zu �ndern.
 *
 * \section Elemente des Traits
 *  \type	double_type	Kann das Ergenis einer Multiplication des Typs T aufnehmen
 *  \value	min_value	kleinster darstellbarer Wert (int: Negative, float/double: kleinste pos Zahl)
 *  \value	max_value	gr��te darstellbarer Wert
 * 
 *  \section Benutzen
 *   \subsection Einen Typ (T_DOUBLE) definieren, in den das Ergebnis von (T * T) passt
 *    \code
 *     typedef typename arithmetic_traits<T>::double_type T_DOUBLE;
 *    \endcode
 *   \subsection min/max Value des Typ erhalten
 *    \code
 *     T min = arithmetic_traits<T>::min_value;
 *     T max = arithmetic_traits<T>::max_value;
 *    \endcode
 *
 * \version	$Id$
 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
/*@{*/



#include <inttypes.h>
#include <math.h> 
#include "typet.h"


// nutzen des Defaulttemplate verursacht einen Compilererror 
// (gewollt, besser als RuntimeError)
template <typename T> struct arithmetic_traits {
	static const bool isInteger = false;
	static const bool isFloat = false;

};
template <> struct arithmetic_traits<double> {
	typedef double double_type;
	static const double min_value = 2.22507e-308;
	static const double max_value = 1.79769e+308;
	static const bool isInteger = false;
	static const bool isFloat = true;
};
template <>  struct arithmetic_traits<float> {
	typedef float double_type;
	static const float min_value = 1.17549e-38;
	static const float max_value = 3.40282e+38;
	static const bool isInteger = false;
	static const bool isFloat = true;
};
template <>  struct arithmetic_traits<int32_t> {
	typedef float double_type; // int64_t ist auch nur ein int32_t
	static const int32_t min_value = -0x80000000;
	static const int32_t max_value =  0x7FFFFFFF;
	static const uint8_t digits10 = 11;
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = true;
};
template <>  struct arithmetic_traits<uint32_t> {
	typedef float double_type; // int64_t ist auch nur ein int32_t
	static const uint32_t min_value =  0x0;
	static const uint32_t max_value =  0xFFFFFFFF;
	static const uint8_t digits10 = 10;
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = false;
};
template <>  struct arithmetic_traits<int16_t> {
	typedef int32_t double_type;
	static const int16_t min_value = -0x8000;
	static const int16_t max_value =  0x7FFF;
	static const uint8_t digits10 = 6; // inc. sigh
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = true;
};
template <>  struct arithmetic_traits<uint16_t> {
	typedef uint32_t double_type;
	static const int16_t min_value =  0x0;
	static const int16_t max_value =  0xFFFF;
	static const uint8_t digits10 = 5;
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = false;
};
template <>  struct arithmetic_traits<int8_t> {
	typedef int16_t double_type;
	static const int8_t min_value = -0x80;
	static const int8_t max_value =  0x7F;
	static const uint8_t digits10 = 4; // inc sign
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = false;
};
template <>  struct arithmetic_traits<uint8_t> {
	typedef uint16_t double_type;
	static const int8_t min_value = 0;
	static const int8_t max_value = 0xFF;
	static const uint8_t digits10 = 3;
	static const bool isInteger = true;
	static const bool isFloat = false;
	static const bool isSigned = false;
};
/*@}*/
#endif	// ARITHMETIC_TRAITS_H

