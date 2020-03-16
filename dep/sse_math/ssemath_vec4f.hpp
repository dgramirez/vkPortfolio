#ifndef SSE_MATH_LIBRARY_VEC4F
#define SSE_MATH_LIBRARY_VEC4F

namespace sml {

	struct vec4f{

		union {
			__m128 m128;
			float e[4];
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			struct { float s, t, p, q; };
		};

		//Constructors and Assignment
		vec4f() {
			//Set the m128 value to 0
			m128 = _mm_setzero_ps();
		}
		vec4f(const float& _x, const float& _y, const float& _z, const float& _w) {
			//Set the m128 value to xyzw
			m128 = _mm_set_ps(_w, _z, _y, _x);
		}
		vec4f(const vec4f& _vector) {
			//Set the m128 value to the other vector's m128 value
			m128 = _vector.m128;
		}
		vec4f(const float* _vectorFP) {
			//Load the float pointer to the m128 value
			m128 = _mm_load_ps(_vectorFP);
		}
		vec4f(const __m128& _vectorSSE) {
			//Set the m128 value to the parameter m128 value
			m128 = _vectorSSE;
		}
		void operator=(const vec4f& _vector) {
			//Assign the m128 value to the parameter vector's m128 value
			m128 = _vector.m128;
		}
		void operator=(const float* _vectorFP) {
			//Assign the m128 to the float pointer loaded in as a m128
			m128 = _mm_load_ps(_vectorFP);
		}
		void operator=(const __m128& _vectorSSE) {
			//Assign the m128 value to the m128 parameter value
			m128 = _vectorSSE;
		}
		void Set(const float& _x, const float& _y, const float& _z, const float& _w) {
			//Set the m128 value to xyzw
			m128 = _mm_set_ps(_w, _z, _y, _x);
		}
		void Set(const vec4f& _vector) {
			//Set the m128 value to the other vector's m128 value
			m128 = _vector.m128;
		}
		void Set(const float* _vectorFP) {
			//Load the float pointer to the m128 value
			m128 = _mm_load_ps(_vectorFP);
		}
		void Set(const __m128& _vectorSSE) {
			//Set the m128 value to the parameter m128 value
			m128 = _vectorSSE;
		}

		//Vec4 Absolute Value
		void Abs() {
			//Toggle the negative bit
			m128 = _mm_and_ps(m128, SSE_POS_NAN);
		}

		//Equality Check
		bool IsZero(const float& _epsilon = SSE_EPSILON) const {
			//Return the Comparison of the 0 vector to the float pointer parameter (0 == same bit value)
			return M128CompareCheck(_mm_cmple_ps(m128, _mm_set1_ps(_epsilon)));
		}
		bool IsEqual(const vec4f& _vector, const float& _epsilon = SSE_EPSILON) const {
			//Exact Equality Check
			__m128 eq = _mm_cmpeq_ps(m128, _vector.m128);
			if (M128CompareCheck(eq))
				return true;

			//Absolute Value the vectors
			__m128 absV1 = sml::VectorAbs(m128);
			__m128 absV2 = sml::VectorAbs(_vector.m128);
			__m128 diff =  sml::VectorAbs(_mm_sub_ps(_vector.m128, m128));
			__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

			//Relative Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
			if (M128CompareCheck(eq))
				return true;

			//Near-Zero Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
			return (M128CompareCheck(eq));
		}
		bool IsEqual(const float* _vectorFP, const float& _epsilon = SSE_EPSILON) const {
			//Cache Floating point Load
			__m128 fp = _mm_load_ps(_vectorFP);
			
				//Exact Equality Check
			__m128 eq = _mm_cmpeq_ps(m128, fp);
			if (M128CompareCheck(eq))
				return true;

			//Absolute Value the vectors
			__m128 absV1 = sml::VectorAbs(m128);
			__m128 absV2 = sml::VectorAbs(fp);
			__m128 diff = sml::VectorAbs(_mm_sub_ps(fp, m128));
			__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

			//Relative Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
			if (M128CompareCheck(eq))
				return true;

			//Near-Zero Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
			return (M128CompareCheck(eq));
		}
		bool IsEqual(const __m128& _vectorSSE, const float& _epsilon = SSE_EPSILON) const {
			//Exact Equality Check
			__m128 eq = _mm_cmpeq_ps(m128, _vectorSSE);
			if (M128CompareCheck(eq))
				return true;

			//Absolute Value the vectors
			__m128 absV1 = sml::VectorAbs(m128);
			__m128 absV2 = sml::VectorAbs(_vectorSSE);
			__m128 diff = sml::VectorAbs(_mm_sub_ps(_vectorSSE, m128));
			__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

			//Relative Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
			if (M128CompareCheck(eq))
				return true;

			//Near-Zero Equality Check
			eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
			return (M128CompareCheck(eq));
		}

		//Equality Check (Operator Overload)
		friend bool operator==(const vec4f& _vector1, const vec4f& _vector2) {
			return _vector1.IsEqual(_vector2);
		}
		friend bool operator==(const vec4f& _vector, const float* _vectorFP) {
			return _vector.IsEqual(_vectorFP);
		}
		friend bool operator==(const vec4f& _vector, const __m128& _vectorSSE) {
			return _vector.IsEqual(_vectorSSE);
		}
		friend bool operator==(const float* _vectorFP, const vec4f& _vector) {
			return _vector.IsEqual(_vectorFP);
		}
		friend bool operator==(const __m128& _vectorSSE, const vec4f& _vector) {
			return _vector.IsEqual(_vectorSSE);
		}

		//Inequality Check (Operator Overload)
		friend bool operator!=(const vec4f& _vector1, const vec4f& _vector2) {
			return !_vector1.IsEqual(_vector2);
		}
		friend bool operator!=(const vec4f& _vector, const float* _vectorFP) {
			return !_vector.IsEqual(_vectorFP);
		}
		friend bool operator!=(const vec4f& _vector, const __m128& _vectorSSE) {
			return !_vector.IsEqual(_vectorSSE);
		}
		friend bool operator!=(const float* _vectorFP, const vec4f& _vector) {
			return !_vector.IsEqual(_vectorFP);
		}
		friend bool operator!=(const __m128& _vectorSSE, const vec4f& _vector) {
			return !_vector.IsEqual(_vectorSSE);
		}

		//Vector-Vector Addition (Self & Self Operator Overloads)
		void Add(const vec4f& _vector) {
			m128 = _mm_add_ps(m128, _vector.m128);
		}
		void Add(const float* _vectorFP) {
			m128 = _mm_add_ps(m128, _mm_load_ps(_vectorFP));
		}
		void Add(const __m128& _vectorSSE) {
			m128 = _mm_add_ps(m128, _vectorSSE);
		}
		void operator+=(const vec4f& _vector) {
			Add(_vector);
		}
		void operator+=(const float* _vectorFP) {
			Add(_vectorFP);
		}
		void operator+=(const __m128& _vectorSSE) {
			Add(_vectorSSE);
		}

		//Vector-Vector Addition (Operator Overloads)
		friend vec4f operator+(const vec4f& _vector1, const vec4f& _vector2) {
			return _mm_add_ps(_vector1.m128, _vector2.m128);
		}
		friend vec4f operator+(const vec4f& _vector, const float* _vectorFP) {
			return _mm_add_ps(_vector.m128, _mm_load_ps(_vectorFP));
		}
		friend vec4f operator+(const vec4f& _vector, const __m128& _vectorSSE) {
			return _mm_add_ps(_vector.m128, _vectorSSE);
		}
		friend vec4f operator+(const float* _vectorFP, const vec4f& _vector) {
			return _mm_add_ps(_mm_load_ps(_vectorFP), _vector.m128);
		}
		friend vec4f operator+(const __m128& _vectorSSE, const vec4f& _vector) {
			return _mm_add_ps(_vectorSSE, _vector.m128);
		}

		//Vector-Vector Subtraction (Self & Self Operator Overloads)
		void Sub(const vec4f& _vector) {
			m128 = _mm_sub_ps(m128, _vector.m128);
		}
		void Sub(const float* _vectorFP) {
			m128 = _mm_sub_ps(m128, _mm_load_ps(_vectorFP));
		}
		void Sub(const __m128& _vectorSSE) {
			m128 = _mm_sub_ps(m128, _vectorSSE);
		}
		void operator-=(const vec4f& _vector) {
			Sub(_vector);
		}
		void operator-=(const float* _vectorFP) {
			Sub(_vectorFP);
		}
		void operator-=(const __m128& _vectorSSE) {
			Sub(_vectorSSE);
		}

		//Vector-Vector Subtraction (Operator Overloads)
		friend vec4f operator-(const vec4f& _vector1, const vec4f& _vector2) {
			return _mm_sub_ps(_vector1.m128, _vector2.m128);
		}
		friend vec4f operator-(const vec4f& _vector, const float* _vectorFP) {
			return _mm_sub_ps(_vector.m128, _mm_load_ps(_vectorFP));
		}
		friend vec4f operator-(const vec4f& _vector, const __m128& _vectorSSE) {
			return _mm_sub_ps(_vector.m128, _vectorSSE);
		}
		friend vec4f operator-(const float* _vectorFP, const vec4f& _vector) {
			return _mm_sub_ps(_mm_load_ps(_vectorFP), _vector.m128);
		}
		friend vec4f operator-(const __m128& _vectorSSE, const vec4f& _vector) {
			return _mm_sub_ps(_vectorSSE, _vector.m128);
		}

		//Vector-Scalar Multiply (Self & Self Operator Overload)
		void Mul(const float& _scalar) {
			m128 = _mm_mul_ps(m128, _mm_set1_ps(_scalar));
		}
		void operator*=(const float& _scalar) {
			Mul(_scalar);
		}

		//Vector-Scalar Multiply (Operator Overload)
		friend vec4f operator*(const vec4f& _vector, const float& _scalar) {
			return _mm_mul_ps(_vector.m128, _mm_set1_ps(_scalar));
		}
		friend vec4f operator*(const float& _scalar, const vec4f& _vector) {
			return _mm_mul_ps(_mm_set1_ps(_scalar), _vector.m128);
		}

		//Vector-Scalar Divide (Self & Self Operator Overload)
		void Div(const float& _scalar) {
			m128 = _mm_div_ps(m128, _mm_set1_ps(_scalar));
		}
		void operator/=(const float& _scalar) {
			Div(_scalar);
		}

		//Vector-Scalar Divide (Operator Overload)
		friend vec4f operator/(const vec4f& _vector, const float& _scalar) {
			return _mm_div_ps(_vector.m128, _mm_set1_ps(_scalar));
		}

		//Vector Negate
		void Negate() {
			m128 = _mm_xor_ps(m128, SSE_NEG_ZERO);
		}
		vec4f operator-() {
			return _mm_xor_ps(m128, SSE_NEG_ZERO);
		}

		//Other Vector Math
		void Min(const vec4f& _vector) {
			m128 = _mm_min_ps(m128, _vector.m128);
		}
		void Min(const float* _vectorFP) {
			m128 = _mm_min_ps(m128, _mm_load_ps(_vectorFP));
		}
		void Min(const __m128& _vectorSSE) {
			m128 = _mm_min_ps(m128, _vectorSSE);
		}

		void Max(const vec4f& _vector) {
			m128 = _mm_max_ps(m128, _vector.m128);
		}
		void Max(const float* _vectorFP) {
			m128 = _mm_max_ps(m128, _mm_load_ps(_vectorFP));
		}
		void Max(const __m128& _vectorSSE) {
			m128 = _mm_max_ps(m128, _vectorSSE);
		}

		void Average(const vec4f& _vector) {
			m128 = _mm_mul_ps(_mm_add_ps(m128, _vector.m128), _mm_set1_ps(0.5f));
		}
		void Average(const float* _vectorFP) {
			m128 = _mm_mul_ps(_mm_add_ps(m128, _mm_load_ps(_vectorFP)), _mm_set1_ps(0.5f));
		}
		void Average(const __m128& _vectorSSE) {
			m128 = _mm_mul_ps(_mm_add_ps(m128, _vectorSSE), _mm_set1_ps(0.5f));
		}

		float Length() const {
			return sqrtf(M128AddComponents(_mm_mul_ps(m128, m128)));
		}
		float LengthSq() const {
			return M128AddComponents(_mm_mul_ps(m128, m128));
		}
		void Normalize() {
			//Get Length Squared
			float l = LengthSq();
			if (l == 0.0f)
				m128 = _mm_setzero_ps();
			else {
				l = 1 / sqrtf(l);
				m128 = _mm_mul_ps(m128, _mm_set1_ps(l));
			}
		}
		void Homogenize() {
			if (IsZero())
				m128 = _mm_setzero_ps();
			else
				m128 = _mm_div_ps(m128, _mm_shuffle_ps(m128, m128, SSE_W));
		}

		//Vector Dot Product
		float Dot(const vec4f& _vector) const {
			return M128AddComponents(_mm_mul_ps(m128, _vector.m128));
		}
		float Dot(const float* _vectorFP) const {
			return M128AddComponents(_mm_mul_ps(m128, _mm_load_ps(_vectorFP)));
		}
		float Dot(const __m128& _vectorSSE) const {
			return M128AddComponents(_mm_mul_ps(m128, _vectorSSE));
		}

		//Vector Dot Product (Operator Overloads)
		friend float operator*(const vec4f& _vector1, const vec4f& _vector2) {
			return M128AddComponents(_mm_mul_ps(_vector1.m128, _vector2.m128));
		}
		friend float operator*(const vec4f& _vector, const float* _vectorFP) {
			return M128AddComponents(_mm_mul_ps(_vector.m128, _mm_load_ps(_vectorFP)));
		}
		friend float operator*(const vec4f& _vector, const __m128& _vectorSSE) {
			return M128AddComponents(_mm_mul_ps(_vector.m128, _vectorSSE));
		}
		friend float operator*(const float* _vectorFP, const vec4f& _vector) {
			return M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP), _vector.m128));
		}
		friend float operator*(const __m128& _vectorSSE, const vec4f& _vector) {
			return M128AddComponents(_mm_mul_ps(_vectorSSE, _vector.m128));
		}

		//Vector Cross Product (Self & Self Operator Overloads)
		void Cross(const vec4f& _vector) {
			m128 = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP1), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP2), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1))
			);
		}
		void Cross(const float* _vectorFP) {
			__m128 fp = _mm_load_ps(_vectorFP);
			m128 = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP1), _mm_shuffle_ps(fp, fp, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP2), _mm_shuffle_ps(fp, fp, CROSS_FLIP1))
			);
		}
		void Cross(const __m128& _vectorSSE) {
			m128 = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP1), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(m128, m128, CROSS_FLIP2), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1))
			);
		}
		void operator^=(const vec4f& _vector) {
			Cross(_vector);
		}
		void operator^=(const float* _vectorFP) {
			Cross(_vectorFP);
		}
		void operator^=(const __m128& _vectorSSE) {
			Cross(_vectorSSE);
		}

		//Vector Cross Product (Operator Overloads)
		friend vec4f operator^(const vec4f& _vector1, const vec4f& _vector2) {
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(_vector1.m128, _vector1.m128, CROSS_FLIP1), _mm_shuffle_ps(_vector2.m128, _vector2.m128, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(_vector1.m128, _vector1.m128, CROSS_FLIP2), _mm_shuffle_ps(_vector2.m128, _vector2.m128, CROSS_FLIP1))
			);
		}
		friend vec4f operator^(const vec4f& _vector, const float* _vectorFP) {
			__m128 fp = _mm_load_ps(_vectorFP);
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1), _mm_shuffle_ps(fp, fp, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2), _mm_shuffle_ps(fp, fp, CROSS_FLIP1))
			);
		}
		friend vec4f operator^(const vec4f& _vector, const __m128& _vectorSSE) {
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1))
			);
		}
		friend vec4f operator^(const float* _vectorFP, const vec4f& _vector) {
			__m128 fp = _mm_load_ps(_vectorFP);
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP1), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP2), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1))
			);
		}
		friend vec4f operator^(const __m128& _vectorSSE, const vec4f& _vector) {
			return _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2)),
				_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1))
			);
		}

		//Vector Angle Between
		float AngleBetween(const vec4f& _vector) const {
			//Get the Two Lengths Multiplied (Zero Check)
			float bot = Length() * _vector.Length();
			if (bot < FLT_EPSILON)
				return 0.0f;
			return acosf(Dot(_vector) / bot);
		}
		float AngleBetween(const float* _vectorFP) const {
			//Get the Two Lengths Multiplied (Zero Check)
			float bot = Length() * sml::Length(_vectorFP);
			if (bot < FLT_EPSILON)
				return 0.0f;
			return acosf(Dot(_vectorFP) / bot);
		}
		float AngleBetween(const __m128& _vectorSSE) const {
			//Get the Two Lengths Multiplied (Zero Check)
			float bot = Length() * sml::Length(_vectorSSE);
			if (bot < FLT_EPSILON)
				return 0.0f;
			return acosf(Dot(_vectorSSE) / bot);
		}

		//Vector Component
		float Component(const vec4f& _vector) const {
			return Dot(sml::Normalize(_vector.m128));
		}
		float Component(const float* _vectorFP) const {
			return Dot(sml::Normalize(_vectorFP));
		}
		float Component(const __m128& _vectorSSE) const {
			return Dot(sml::Normalize(_vectorSSE));
		}

		//Vector Project
		void Project(const vec4f& _vector) {
			__m128 n = sml::Normalize(_vector.m128);
			m128 = sml::VectorMul(n, Dot(n));
		}
		void Project(const float* _vectorFP) {
			__m128 n = sml::Normalize(_vectorFP);
			m128 = sml::VectorMul(n, Dot(n));
		}
		void Project(const __m128& _vectorSSE) {
			__m128 n = sml::Normalize(_vectorSSE);
			m128 = sml::VectorMul(n, Dot(n));
		}

		//Vector Reflect
		void Reflect(const vec4f& _vector) {
			if (_vector.IsZero())
				Negate();
			else
				m128 = _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(m128, _vector.m128), _mm_set1_ps(-2.0f)), m128), SSE_NEG_ZERO);
		}
		void Reflect(const float* _vectorFP) {
			if (sml::VectorIsZero(_vectorFP))
				Negate();
			else
				m128 = _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(m128, _vectorFP), _mm_set1_ps(-2.0f)), m128), SSE_NEG_ZERO);
		}
		void Reflect(const __m128& _vectorSSE) {
			if (sml::VectorIsZero(_vectorSSE))
				Negate();
			else
				m128 = _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(m128, _vectorSSE), _mm_set1_ps(-2.0f)), m128), SSE_NEG_ZERO);
		}
	};

	//Vec4 Absolute Value
	static __m128 VectorAbs(const vec4f& _vector) {
		//Remove the negative bit
		return _mm_and_ps(_vector.m128, SSE_POS_NAN);
	}

	//Set
	static __m128 VectorSet(const vec4f& _vector) {
		return _vector.m128;
	}

	//Equality Check (Zero)
	static bool VectorIsZero(const vec4f& _vector, const float& _epsilon = SSE_EPSILON) {
		//Return the Comparison of the 0 vector to the m128 parameter (0 == same bit value)
		return M128CompareCheck(_mm_cmple_ps(_vector.m128, _mm_set1_ps(_epsilon)));
	}

	//Equality Check
	static bool VectorIsEqual(const vec4f& _vector1, const vec4f& _vector2, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 eq = _mm_cmpeq_ps(_vector1.m128, _vector2.m128);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = sml::VectorAbs(_vector1.m128);
		__m128 absV2 = sml::VectorAbs(_vector2.m128);
		__m128 diff =  sml::VectorAbs(_mm_sub_ps(_vector2.m128, _vector1.m128));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const vec4f& _vector, const float* _vectorFP, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 fp = _mm_load_ps(_vectorFP);
		__m128 eq = _mm_cmpeq_ps(_vector.m128, fp);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = sml::VectorAbs(_vector.m128);
		__m128 absV2 = sml::VectorAbs(fp);
		__m128 diff =  sml::VectorAbs(_mm_sub_ps(fp, _vector.m128));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const vec4f& _vector, const __m128& _vectorSSE, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 eq = _mm_cmpeq_ps(_vector.m128, _vectorSSE);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = sml::VectorAbs(_vector.m128);
		__m128 absV2 = sml::VectorAbs(_vectorSSE);
		__m128 diff =  sml::VectorAbs(_mm_sub_ps(_vectorSSE, _vector.m128));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const float* _vectorFP, const vec4f& _vector, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 fp = _mm_load_ps(_vectorFP);
		__m128 eq = _mm_cmpeq_ps(fp, _vector.m128);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = sml::VectorAbs(fp);
		__m128 absV2 = sml::VectorAbs(_vector.m128);
		__m128 diff =  sml::VectorAbs(_mm_sub_ps(_vector.m128, fp));
		__m128 relDiv = _mm_min_ps(_mm_add_ps(absV1, absV2), _mm_set1_ps(FLT_MAX));

		//Relative Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(_mm_div_ps(diff, relDiv), _mm_set1_ps(_epsilon)));
		if (M128CompareCheck(eq))
			return true;

		//Near-Zero Equality Check
		eq = _mm_or_ps(eq, _mm_cmple_ps(diff, _mm_set1_ps(_epsilon)));
		return (M128CompareCheck(eq));
	}
	static bool VectorIsEqual(const __m128& _vectorSSE, const vec4f& _vector, const float& _epsilon = SSE_EPSILON) {
		//Exact Equality Check
		__m128 eq = _mm_cmpeq_ps(_vectorSSE, _vector.m128);
		if (M128CompareCheck(eq))
			return true;

		//Absolute Value the vectors
		__m128 absV1 = sml::VectorAbs(_vectorSSE);
		__m128 absV2 = sml::VectorAbs(_vector.m128);
		__m128 diff =  sml::VectorAbs(_mm_sub_ps(_vector.m128, _vectorSSE));
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
	static __m128 VectorAdd(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_add_ps(_vector1.m128, _vector2.m128);
	}
	static __m128 VectorAdd(const vec4f& _vector, const float* _vectorFP) {
		return _mm_add_ps(_vector.m128, _mm_load_ps(_vectorFP));
	}
	static __m128 VectorAdd(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_add_ps(_vector.m128, _vectorSSE);
	}
	static __m128 VectorAdd(const float* _vectorFP, const vec4f& _vector) {
		return _mm_add_ps(_mm_load_ps(_vectorFP), _vector.m128);
	}
	static __m128 VectorAdd(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_add_ps(_vectorSSE, _vector.m128);
	}

	//Vector-Vector Subtraction
	static __m128 VectorSub(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_sub_ps(_vector1.m128, _vector2.m128);
	}
	static __m128 VectorSub(const vec4f& _vector, const float* _vectorFP) {
		return _mm_sub_ps(_vector.m128, _mm_load_ps(_vectorFP));
	}
	static __m128 VectorSub(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_sub_ps(_vector.m128, _vectorSSE);
	}
	static __m128 VectorSub(const float* _vectorFP, const vec4f& _vector) {
		return _mm_sub_ps(_mm_load_ps(_vectorFP), _vector.m128);
	}
	static __m128 VectorSub(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_sub_ps(_vectorSSE, _vector.m128);
	}

	//Vector-Scalar Multiply
	static __m128 VectorMul(const vec4f& _vector, const float& _scalar) {
		return _mm_mul_ps(_vector.m128, _mm_set1_ps(_scalar));
	}
	static __m128 VectorMul(const float& _scalar, const vec4f& _vector) {
		return _mm_mul_ps(_mm_set1_ps(_scalar), _vector.m128);
	}

	//Vector-Scalar Divide (Static & Global Operator Overload)
	static __m128 VectorDiv(const vec4f& _vector, const float& _scalar) {
		return _mm_div_ps(_vector.m128, _mm_set1_ps(_scalar));
	}

	//Vector Negate
	static __m128 VectorNegate(const vec4f& _vector) {
		return _mm_xor_ps(_vector.m128, SSE_NEG_ZERO);
	}

	//Vector Minimum (Per Component)
	static __m128 Min(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_min_ps(_vector1.m128, _vector2.m128);
	}
	static __m128 Min(const vec4f& _vector, const float* _vectorFP) {
		return _mm_min_ps(_vector.m128, _mm_load_ps(_vectorFP));
	}
	static __m128 Min(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_min_ps(_vector.m128, _vectorSSE);
	}
	static __m128 Min(const float* _vectorFP, const vec4f& _vector) {
		return _mm_min_ps(_mm_load_ps(_vectorFP), _vector.m128);
	}
	static __m128 Min(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_min_ps(_vectorSSE, _vector.m128);
	}

	//Vector Maximum (Per Component)
	static __m128 Max(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_max_ps(_vector1.m128, _vector2.m128);
	}
	static __m128 Max(const vec4f& _vector, const float* _vectorFP) {
		return _mm_max_ps(_vector.m128, _mm_load_ps(_vectorFP));
	}
	static __m128 Max(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_max_ps(_vector.m128, _vectorSSE);
	}
	static __m128 Max(const float* _vectorFP, const vec4f& _vector) {
		return _mm_max_ps(_mm_load_ps(_vectorFP), _vector.m128);
	}
	static __m128 Max(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_max_ps(_vectorSSE, _vector.m128);
	}

	//Vector Average (Per Component)
	static __m128 Average(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_mul_ps(_mm_add_ps(_vector1.m128, _vector2.m128), _mm_set1_ps(0.5f));
	}
	static __m128 Average(const vec4f& _vector, const float* _vectorFP) {
		return _mm_mul_ps(_mm_add_ps(_vector.m128, _mm_load_ps(_vectorFP)), _mm_set1_ps(0.5f));
	}
	static __m128 Average(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_mul_ps(_mm_add_ps(_vector.m128, _vectorSSE), _mm_set1_ps(0.5f));
	}
	static __m128 Average(const float* _vectorFP, const vec4f& _vector) {
		return _mm_mul_ps(_mm_add_ps(_mm_load_ps(_vectorFP), _vector.m128), _mm_set1_ps(0.5f));
	}
	static __m128 Average(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_mul_ps(_mm_add_ps(_vectorSSE, _vector.m128), _mm_set1_ps(0.5f));
	}

	//Vector Length
	static float Length(const vec4f& _vector) {
		return sqrtf(M128AddComponents(_mm_mul_ps(_vector.m128, _vector.m128)));
	}
	static float LengthSq(const vec4f& _vector) {
		return M128AddComponents(_mm_mul_ps(_vector.m128, _vector.m128));
	}

	//Vector Dot Product
	static float Dot(const vec4f& _vector1, const vec4f& _vector2) {
		return M128AddComponents(_mm_mul_ps(_vector1.m128, _vector2.m128));
	}
	static float Dot(const vec4f& _vector, const float* _vectorFP) {
		return M128AddComponents(_mm_mul_ps(_vector.m128, _mm_load_ps(_vectorFP)));
	}
	static float Dot(const vec4f& _vector, const __m128& _vectorSSE) {
		return M128AddComponents(_mm_mul_ps(_vector.m128, _vectorSSE));
	}
	static float Dot(const float* _vectorFP, const vec4f& _vector) {
		return M128AddComponents(_mm_mul_ps(_mm_load_ps(_vectorFP), _vector.m128));
	}
	static float Dot(const __m128& _vectorSSE, const vec4f& _vector) {
		return M128AddComponents(_mm_mul_ps(_vectorSSE, _vector.m128));
	}

	//Vector Cross Product (Static & Global Operator Overloads)
	static __m128 Cross(const vec4f& _vector1, const vec4f& _vector2) {
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vector1.m128, _vector1.m128, CROSS_FLIP1), _mm_shuffle_ps(_vector2.m128, _vector2.m128, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vector1.m128, _vector1.m128, CROSS_FLIP2), _mm_shuffle_ps(_vector2.m128, _vector2.m128, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const vec4f& _vector, const float* _vectorFP) {
		__m128 fp = _mm_load_ps(_vectorFP);
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1), _mm_shuffle_ps(fp, fp, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2), _mm_shuffle_ps(fp, fp, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const vec4f& _vector, const __m128& _vectorSSE) {
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2), _mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const float* _vectorFP, const vec4f& _vector) {
		__m128 fp = _mm_load_ps(_vectorFP);
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP1), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(fp, fp, CROSS_FLIP2), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1))
		);
	}
	static __m128 Cross(const __m128& _vectorSSE, const vec4f& _vector) {
		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP1), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP2)),
			_mm_mul_ps(_mm_shuffle_ps(_vectorSSE, _vectorSSE, CROSS_FLIP2), _mm_shuffle_ps(_vector.m128, _vector.m128, CROSS_FLIP1))
		);
	}

	//Vector Normalize
	static __m128 Normalize(const vec4f& _vector) {
		//Get Length Squared
		float l = _vector.LengthSq();
		if (l < FLT_EPSILON)
			return _mm_setzero_ps();
		else
			return _mm_mul_ps(_vector.m128, _mm_set1_ps(1 / sqrtf(l)));
	}

	//Vector Homogenize (Perspective Divide)
	static __m128 Homogenize(const vec4f& _vector){
		if (_vector.IsZero())
			return _mm_setzero_ps();
		else
			return _mm_div_ps(_vector.m128, _mm_shuffle_ps(_vector.m128, _vector.m128, SSE_W));
	}

	//Vector Angle Between
	static float AngleBetween(const vec4f& _vector1, const vec4f& _vector2) {
		float bot = _vector1.Length() * _vector2.Length();
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(_vector1.Dot(_vector2) / bot);
	}
	static float AngleBetween(const vec4f& _vector, const float* _vectorFP) {
		float bot = _vector.Length() * sml::Length(_vectorFP);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(_vector.Dot(_vectorFP) / bot);
	}
	static float AngleBetween(const vec4f& _vector, const __m128& _vectorSSE) {
		float bot = _vector.Length() * sml::Length(_vectorSSE);
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(_vector.Dot(_vectorSSE) / bot);
	}
	static float AngleBetween(const float* _vectorFP, const vec4f& _vector) {
		float bot = sml::Length(_vectorFP) * _vector.Length();
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(sml::Dot(_vectorFP, _vector.m128) / bot);
	}
	static float AngleBetween(const __m128& _vectorSSE, const vec4f& _vector) {
		float bot = sml::Length(_vectorSSE) * _vector.Length();
		if (bot < FLT_EPSILON)
			return 0.0f;
		return acosf(sml::Dot(_vectorSSE, _vector.m128) / bot);
	}

	//Vector Component
	static float Component(const vec4f& _vector1, const vec4f& _vector2) {
		return _vector1.Dot(sml::Normalize(_vector2));
	}
	static float Component(const vec4f& _vector, const float* _vectorFP) {
		return _vector.Dot(sml::Normalize(_vectorFP));
	}
	static float Component(const vec4f& _vector, const __m128& _vectorSSE) {
		return _vector.Dot(sml::Normalize(_vectorSSE));
	}
	static float Component(const float* _vectorFP, const vec4f& _vector) {
		return sml::Dot(_vectorFP, sml::Normalize(_vector.m128));
	}
	static float Component(const __m128& _vectorSSE, const vec4f& _vector) {
		return sml::Dot(_vectorSSE, sml::Normalize(_vector.m128));
	}

	//Vector Project
	static __m128 Project(const vec4f& _vector1, const vec4f& _vector2) {
		__m128 n = sml::Normalize(_vector2.m128); return sml::VectorMul(n, _vector1.Dot(n));
	}
	static __m128 Project(const vec4f& _vector, const float* _vectorFP) {
		__m128 n = sml::Normalize(_vectorFP); return sml::VectorMul(n, _vector.Dot(n));
	}
	static __m128 Project(const vec4f& _vector, const __m128& _vectorSSE) {
		__m128 n = sml::Normalize(_vectorSSE); return sml::VectorMul(n, _vector.Dot(n));
	}
	static __m128 Project(const float* _vectorFP, const vec4f& _vector) {
		__m128 n = sml::Normalize(_vector.m128); return sml::VectorMul(n, sml::Dot(_vectorFP, n));
	}
	static __m128 Project(const __m128& _vectorSSE, const vec4f& _vector) {
		__m128 n = sml::Normalize(_vector.m128); return sml::VectorMul(n, sml::Dot(_vectorSSE, n));
	}

	//Vector Reflect
	static __m128 Reflect(const vec4f& _vector1, const vec4f& _vector2) {
		if (_vector2.IsZero())
			return sml::VectorNegate(_vector1.m128);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(_vector1.m128, _vector2.m128), _mm_set1_ps(-2.0f)), _vector1.m128), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const vec4f& _vector, const float* _vectorFP) {
		if (sml::VectorIsZero(_vectorFP))
			return sml::VectorNegate(_vector.m128);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(_vector.m128, _mm_load_ps(_vectorFP)), _mm_set1_ps(-2.0f)), _vector.m128), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const vec4f& _vector, const __m128& _vectorSSE) {
		if (sml::VectorIsZero(_vectorSSE))
			return sml::VectorNegate(_vector.m128);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(_vector.m128, _vectorSSE), _mm_set1_ps(-2.0f)), _vector.m128), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const float* _vectorFP, const vec4f& _vector) {
		__m128 fp = _mm_load_ps(_vectorFP);
		if (_vector.IsZero())
			return sml::VectorNegate(_vectorFP);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(fp, _vector.m128), _mm_set1_ps(-2.0f)), fp), SSE_NEG_ZERO);
	}
	static __m128 Reflect(const __m128& _vectorSSE, const vec4f& _vector) {
		if (_vector.IsZero())
			return sml::VectorNegate(_vectorSSE);
		return _mm_xor_ps(_mm_add_ps(_mm_mul_ps(sml::Project(_vectorSSE, _vector.m128), _mm_set1_ps(-2.0f)), _vectorSSE), SSE_NEG_ZERO);
	}

#ifdef VEC4_FLOATS_GLOBAL
	typedef vec4f vec4;
#endif
}


#endif