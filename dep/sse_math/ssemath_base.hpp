#ifndef SSE_MATH_LIBRARY_COMMON
#define SSE_MATH_LIBRARY_COMMON

#include <xmmintrin.h>
#include <math.h>
#include <float.h>

//Defines for Cross Shuffle
#define CROSS_FLIP1 0xC9	//11 00 10 01, or 3 0 2 1
#define CROSS_FLIP2 0xD2	//11 01 00 10, or 3 1 0 2
#define SSE_W 0xFF			//11 11 11 11, or 3 3 3 3

namespace sml {

	//Defines for Global Values
	static const __m128 SSE_POS_NAN = _mm_or_ps(_mm_set1_ps(FLT_MAX), _mm_set1_ps(FLT_MIN));
	static const __m128 SSE_NEG_ZERO = _mm_set1_ps(-0.0f);
	static const float SSE_EPSILON = FLT_EPSILON * 2;

	//Absolute Value
	static __m128 VectorAbs(const float* _vectorFP) {
		//Remove the negative bit
		return _mm_and_ps(_mm_load_ps(_vectorFP), SSE_POS_NAN);
	}
	static __m128 VectorAbs(const __m128& _vectorSSE) {
		//Remove the negative bit
		return _mm_and_ps(_vectorSSE, SSE_POS_NAN);
	}

	//m128 Helpers
	static bool M128CompareCheck(const __m128& _vectorSSE) {
		//Create Temporary variables (one for shuffe, other for storing)
		__m128 tmp = _mm_and_ps(_mm_movehl_ps(_vectorSSE, _vectorSSE), _vectorSSE);
		float ret;
		_mm_store_ss(&ret, _mm_and_ps(tmp, _mm_shuffle_ps(tmp, tmp, 1)));

		//Return the result
		return static_cast<bool>(ret);
	}
	static float M128AddComponents(const __m128& _vectorSSE) {
		//Create Temporary variables (one for shuffe, other for storing)
		__m128 tmp = _mm_add_ps(_mm_movehl_ps(_vectorSSE, _vectorSSE), _vectorSSE);
		float ret;
		_mm_store_ss(&ret, _mm_add_ss(tmp, _mm_shuffle_ps(tmp, tmp, 1)));

		//Return the result
		return ret;
	}

	//Set
	static __m128 VectorSet(const float& _x, const float& _y, const float& _z, const float& _w) {
		//Setting the values. Note: order is reversed (3, 2, 1, 0)
		return _mm_set_ps(_w, _z, _y, _x);
	}
	static __m128 VectorSet(const float* _vectorFP) {
		//Load the float pointer into an m128
		return _mm_load_ps(_vectorFP);
	}

	//VectorIsZero
	static bool VectorIsZero(const float* _vectorFP, const float& _epsilon = SSE_EPSILON) {
		//Return the Comparison of the 0 vector to the float pointer parameter (0 == same bit value)
		return M128CompareCheck(_mm_cmple_ps(_mm_load_ps(_vectorFP), _mm_set1_ps(_epsilon)));
	}
	static bool VectorIsZero(const __m128& _vectorSSE, const float& _epsilon = SSE_EPSILON) {
		//Return the Comparison of the 0 vector to the m128 parameter (0 == same bit value)
		return M128CompareCheck(_mm_cmple_ps(_vectorSSE, _mm_set1_ps(_epsilon)));
	}

	//IsEqual
	static bool VectorIsEqual(const float* _vectorFP1, const float* _vectorFP2, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 fp1 = _mm_load_ps(_vectorFP1);
		__m128 fp2 = _mm_load_ps(_vectorFP2);
		__m128 eq = _mm_cmpeq_ps(fp1, fp2);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1  = VectorAbs(fp1);
		__m128 absV2  = VectorAbs(fp2);
		__m128 diff   = VectorAbs(_mm_sub_ps(fp2, fp1));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2) , _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const __m128& _vectorSSE1, const __m128& _vectorSSE2, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 eq = _mm_cmpeq_ps(_vectorSSE1, _vectorSSE2);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = VectorAbs(_vectorSSE1);
		__m128 absV2 = VectorAbs(_vectorSSE2);
		__m128 diff = VectorAbs(_mm_sub_ps(_vectorSSE2, _vectorSSE1));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const float* _vectorFP, const __m128& _vectorSSE, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 fp = _mm_load_ps(_vectorFP);
		__m128 eq = _mm_cmpeq_ps(fp, _vectorSSE);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = VectorAbs(fp);
		__m128 absV2 = VectorAbs(_vectorSSE);
		__m128 diff = VectorAbs(_mm_sub_ps(_vectorSSE, fp));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const __m128& _vectorSSE, const float* _vectorFP, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 fp = _mm_load_ps(_vectorFP);
		__m128 eq = _mm_cmpeq_ps(_vectorSSE, fp);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = VectorAbs(_vectorSSE);
		__m128 absV2 = VectorAbs(fp);
		__m128 diff = VectorAbs(_mm_sub_ps(fp, _vectorSSE));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}

	//Vector-Vector Addition
	static __m128 VectorAdd(const float* _vectorFP1, const float* _vectorFP2)		{ return _mm_add_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2)); }
	static __m128 VectorAdd(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return _mm_add_ps(_vectorSSE1, _vectorSSE2); }
	static __m128 VectorAdd(const float* _vectorFP, const __m128& _vectorSSE)		{ return _mm_add_ps(_mm_load_ps(_vectorFP), _vectorSSE); }
	static __m128 VectorAdd(const __m128& _vectorSSE, const float* _vectorFP)		{ return _mm_add_ps(_vectorSSE, _mm_load_ps(_vectorFP)); }

	//Vector-Vector Subtraction
	static __m128 VectorSub(const float* _vectorFP1, const float* _vectorFP2)		{ return _mm_sub_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2)); }
	static __m128 VectorSub(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return _mm_sub_ps(_vectorSSE1, _vectorSSE2);; }
	static __m128 VectorSub(const float* _vectorFP, const __m128& _vectorSSE)		{ return _mm_sub_ps(_mm_load_ps(_vectorFP), _vectorSSE); }
	static __m128 VectorSub(const __m128& _vectorSSE, const float* _vectorFP)		{ return _mm_sub_ps(_vectorSSE, _mm_load_ps(_vectorFP)); }

	//Vector-Vector Scalar Multiplication
	static __m128 VectorMul(const float* _vectorFP, const float& _scalar)		{ return _mm_mul_ps(_mm_load_ps(_vectorFP), _mm_set1_ps(_scalar)); }
	static __m128 VectorMul(const float& _scalar, const float* _vectorFP)		{ return _mm_mul_ps(_mm_set1_ps(_scalar), _mm_load_ps(_vectorFP)); }
	static __m128 VectorMul(const __m128& _vectorSSE, const float& _scalar)	{ return _mm_mul_ps(_vectorSSE, _mm_set1_ps(_scalar)); }
	static __m128 VectorMul(const float& _scalar, const __m128& _vectorSSE)	{ return _mm_mul_ps(_mm_set1_ps(_scalar), _vectorSSE); }

	//Vector-Vector Scalar Multiplication
	static __m128 VectorDiv(const float* _vectorFP, const float& _scalar)		{ return _mm_div_ps(_mm_load_ps(_vectorFP), _mm_set1_ps(_scalar)); }
	static __m128 VectorDiv(const __m128& _vectorSSE, const float& _scalar)	{ return _mm_div_ps(_vectorSSE, _mm_set1_ps(_scalar)); }

	//Vector VectorNegate
	static __m128 VectorNegate(const float* _vectorFP)	{ return _mm_xor_ps(_mm_load_ps(_vectorFP), SSE_NEG_ZERO); }
	static __m128 VectorNegate(const __m128& _vectorSSE)	{ return _mm_xor_ps(_vectorSSE, SSE_NEG_ZERO); }

	//Vector Minimum
	static __m128 Min(const float* _vectorFP1, const float* _vectorFP2)		{ return _mm_min_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2)); }
	static __m128 Min(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return _mm_min_ps(_vectorSSE1, _vectorSSE2); }
	static __m128 Min(const float* _vectorFP, const __m128& _vectorSSE)		{ return _mm_min_ps(_mm_load_ps(_vectorFP), _vectorSSE); }
	static __m128 Min(const __m128& _vectorSSE, const float* _vectorFP)		{ return _mm_min_ps(_vectorSSE, _mm_load_ps(_vectorFP)); }

	//Vector Maximum
	static __m128 Max(const float* _vectorFP1, const float* _vectorFP2)		{ return _mm_max_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2)); }
	static __m128 Max(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return _mm_max_ps(_vectorSSE1, _vectorSSE2); }
	static __m128 Max(const float* _vectorFP, const __m128& _vectorSSE)		{ return _mm_max_ps(_mm_load_ps(_vectorFP), _vectorSSE); }
	static __m128 Max(const __m128& _vectorSSE, const float* _vectorFP)		{ return _mm_max_ps(_vectorSSE, _mm_load_ps(_vectorFP)); }

	//Vector Average
	static __m128 Average(const float* _vectorFP1, const float* _vectorFP2)		{ return _mm_mul_ps(_mm_add_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2)), _mm_set1_ps(0.5f)); }
	static __m128 Average(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return _mm_mul_ps(_mm_add_ps(_vectorSSE1, _vectorSSE2), _mm_set1_ps(0.5f)); }
	static __m128 Average(const float* _vectorFP, const __m128& _vectorSSE)		{ return _mm_mul_ps(_mm_add_ps(_mm_load_ps(_vectorFP), _vectorSSE), _mm_set1_ps(0.5f)); }
	static __m128 Average(const __m128& _vectorSSE, const float* _vectorFP)		{ return _mm_mul_ps(_mm_add_ps(_vectorSSE, _mm_load_ps(_vectorFP)), _mm_set1_ps(0.5f)); }

	//Vector Length
	static float Length(const float* _vectorFP)	  { return sqrtf(M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP), _mm_load_ps(_vectorFP)))); }
	static float Length(const __m128& _vectorSSE) { return sqrtf(M128AddComponents(_mm_mul_ps(_vectorSSE, _vectorSSE))); }

	//Vector Length Squared
	static float LengthSq(const float* _vectorFP)	{ return M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP), _mm_load_ps(_vectorFP))); }
	static float LengthSq(const __m128& _vectorSSE)	{ return M128AddComponents(_mm_mul_ps(_vectorSSE, _vectorSSE)); }

	//Vector Dot Product
	static float Dot(const float* _vectorFP1, const float* _vectorFP2)		{ return M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP1), _mm_load_ps(_vectorFP2))); }
	static float Dot(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ return M128AddComponents(_mm_mul_ps(_vectorSSE1, _vectorSSE2)); }
	static float Dot(const float* _vectorFP, const __m128& _vectorSSE)		{ return M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP), _vectorSSE)); }
	static float Dot(const __m128& _vectorSSE, const float* _vectorFP)		{ return M128AddComponents(_mm_mul_ps(_vectorSSE, _mm_load_ps(_vectorFP))); }

	//Vector Cross Product
	static __m128 Cross(const float* _vectorFP1, const float* _vectorFP2) {
		__m128 fp1 = _mm_load_ps(_vectorFP1);
		__m128 fp2 = _mm_load_ps(_vectorFP2);
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(fp1, fp1, CROSS_FLIP1), _mm_shuffle_ps(fp2, fp2, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(fp1, fp1, CROSS_FLIP2), _mm_shuffle_ps(fp2, fp2, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const __m128& _vectorSSE1, const __m128& _vectorSSE2) {
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE1, _vectorSSE1, CROSS_FLIP1), _mm_shuffle_ps(_vectorSSE2, _vectorSSE2, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE1, _vectorSSE1, CROSS_FLIP2), _mm_shuffle_ps(_vectorSSE2, _vectorSSE2, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const float* _vectorFP, const __m128& _vectorSSE) {
		__m128 fp = _mm_load_ps(_vectorFP);
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP1), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP2), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const __m128& _vectorSSE, const float* _vectorFP) {
		__m128 fp = _mm_load_ps(_vectorFP);
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1), _mm_shuffle_ps(fp, fp, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2), _mm_shuffle_ps(fp, fp, CROSS_FLIP1))
		);
	}

	//Vector Normalize
	static __m128 Normalize(const float* _vectorFP) {
		//Get Length Squared
		float l = LengthSq(_vectorFP);
		if (l < FLT_EPSILON)
			return _mm_setzero_ps();
		else
			return _mm_mul_ps(_mm_load_ps(_vectorFP), _mm_set1_ps(1 / sqrtf(l)));
	}
	static __m128 Normalize(const __m128& _vectorSSE) {
		//Get Length Squared
		float l = LengthSq(_vectorSSE);
		if (l < FLT_EPSILON)
			return _mm_setzero_ps();
		else
			return _mm_mul_ps(_vectorSSE, _mm_set1_ps(1 / sqrtf(l)));
	}

	//Vector Homogenize
	static __m128 Homogenize(const float* _vectorFP) {
		if (VectorIsZero(_vectorFP))
			return _mm_setzero_ps();
		else
			return _mm_div_ps(_mm_load_ps(_vectorFP), _mm_set1_ps(_vectorFP[3]));
	}
	static __m128 Homogenize(const __m128& _vectorSSE) {
		if (VectorIsZero(_vectorSSE))
			return _mm_setzero_ps();
		else
			return _mm_div_ps(_vectorSSE, _mm_shuffle_ps(_vectorSSE, _vectorSSE, SSE_W));
	}

	//Vector Angle Between
	static float AngleBetween(const float* _vectorFP1, const float* _vectorFP2) {
		//Get the Two Lengths Multiplied (Zero Check)
		float bot = Length(_vectorFP1) * Length(_vectorFP2);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(Dot(_vectorFP1, _vectorFP2) / bot);
	}
	static float AngleBetween(const __m128& _vectorSSE1, const __m128& _vectorSSE2) {
		//Get the Two Lengths Multiplied (Zero Check)
		float bot = Length(_vectorSSE1) * Length(_vectorSSE2);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(Dot(_vectorSSE1, _vectorSSE2) / bot);
	}
	static float AngleBetween(const float* _vectorFP, const __m128& _vectorSSE) {
		//Get the Two Lengths Multiplied (Zero Check)
		float bot = Length(_vectorFP) * Length(_vectorSSE);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(Dot(_vectorFP, _vectorSSE) / bot);
	}
	static float AngleBetween(const __m128& _vectorSSE, const float* _vectorFP) {
		//Get the Two Lengths Multiplied (Zero Check)
		float bot = Length(_vectorSSE) * Length(_vectorFP);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(Dot(_vectorSSE, _vectorFP) / bot);
	}

	//Vector Component Additions
	static float Component(const float* _vectorFP1, const float* _vectorFP2)	 { return Dot(_vectorFP1, Normalize(_vectorFP2)); }
	static float Component(const __m128& _vectorSSE1, const __m128& _vectorSSE2) { return Dot(_vectorSSE1, Normalize(_vectorSSE2)); }
	static float Component(const float* _vectorFP, const __m128& _vectorSSE)	 { return Dot(_vectorFP, Normalize(_vectorSSE)); }
	static float Component(const __m128& _vectorSSE, const float* _vectorFP)	 { return Dot(_vectorSSE, Normalize(_vectorFP)); }

	//Vector Project Additions
	static __m128 Project(const float* _vectorFP1, const float* _vectorFP2)		{ __m128 n = Normalize(_vectorFP2); return VectorMul(n, Dot(_vectorFP1, n)); }
	static __m128 Project(const __m128& _vectorSSE1, const __m128& _vectorSSE2)	{ __m128 n = Normalize(_vectorSSE2); return VectorMul(n, Dot(_vectorSSE1, n)); }
	static __m128 Project(const float* _vectorFP, const __m128& _vectorSSE)		{ __m128 n = Normalize(_vectorSSE); return VectorMul(n, Dot(_vectorFP, n)); }
	static __m128 Project(const __m128& _vectorSSE, const float* _vectorFP)		{ __m128 n = Normalize(_vectorFP); return VectorMul(n, Dot(_vectorSSE, n)); }

	//Vector Reflect Additions
	static __m128 Reflect(const float* _vectorFP1, const float* _vectorFP2) {
		if (VectorIsZero(_vectorFP2))
			return VectorNegate(_vectorFP1);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(Project(_vectorFP1, _vectorFP2), _mm_set1_ps(-2.0f)), _mm_load_ps(_vectorFP1)), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const __m128& _vectorSSE1, const __m128& _vectorSSE2) {
		if (VectorIsZero(_vectorSSE2))
			return VectorNegate(_vectorSSE1);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(Project(_vectorSSE1, _vectorSSE2), _mm_set1_ps(-2.0f)), _vectorSSE1), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const float* _vectorFP, const __m128& _vectorSSE) {
		if (VectorIsZero(_vectorSSE))
			return VectorNegate(_vectorFP);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(Project(_vectorFP, _vectorSSE), _mm_set1_ps(-2.0f)), _mm_load_ps(_vectorFP)), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const __m128& _vectorSSE, const float* _vectorFP) {
		if (VectorIsZero(_vectorFP))
			return VectorNegate(_vectorSSE);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(Project(_vectorSSE, _vectorFP), _mm_set1_ps(-2.0f)), _vectorSSE), SSE_NEG_ZERO);
	}
}

//Undefining
//#undef CROSS_FLIP1
//#undef CROSS_FLIP2
//#undef SSE_W

#endif