#ifndef SSE_MATH_LIBRARY_MAT4F
#define SSE_MATH_LIBRARY_MAT4F

namespace sml {

	struct mat4f {
		//Union between SSE, float Array and Vectors
		union {
			struct { __m128 m128X, m128Y, m128Z, m128T; };
			__m128 m128[4];
			float e[16];
			struct {
				float
					e11, e12, e13, e14,
					e21, e22, e23, e24,
					e31, e32, e33, e34,
					e41, e42, e43, e44;
			};
		};

		//Constructors and Assignment
		mat4f() {
			m128[0] = m128[1] = m128[2] = m128[3] = _mm_setzero_ps();
		}
		mat4f(const float& _diagonal) {
			m128[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, _diagonal);
			m128[1] = _mm_set_ps(0.0f, 0.0f, _diagonal, 0.0f);
			m128[2] = _mm_set_ps(0.0f, _diagonal, 0.0f, 0.0f);
			m128[3] = _mm_set_ps(  1.0f   , 0.0f, 0.0f, 0.0f);
		}
		mat4f(const __m128& _vectorSSEx, const __m128& _vectorSSEy, const __m128& _vectorSSEz, const __m128& _vectorSSEt) {
			m128[0] = _vectorSSEx;
			m128[1] = _vectorSSEy;
			m128[2] = _vectorSSEz;
			m128[3] = _vectorSSEt;
		}
		mat4f(const float* _vectorFPx, const float* _vectorFPy, const float* _vectorFPz, const float* _vectorFPt) {
			m128[0] = _mm_load_ps(_vectorFPx);
			m128[1] = _mm_load_ps(_vectorFPy);
			m128[2] = _mm_load_ps(_vectorFPz);
			m128[3] = _mm_load_ps(_vectorFPt);
		}
		mat4f(const float* _matrixFP) {
			m128[0] = _mm_load_ps(_matrixFP);
			m128[1] = _mm_load_ps(&_matrixFP[4]);
			m128[2] = _mm_load_ps(&_matrixFP[8]);
			m128[3] = _mm_load_ps(&_matrixFP[12]);
		}
		mat4f(const float& _e11, const float& _e12, const float& _e13, const float& _e14,
			const float& _e21, const float& _e22, const float& _e23, const float& _e24,
			const float& _e31, const float& _e32, const float& _e33, const float& _e34,
			const float& _e41, const float& _e42, const float& _e43, const float& _e44) {
			m128[0] = _mm_set_ps(_e14, _e13, _e12, _e11);
			m128[1] = _mm_set_ps(_e24, _e23, _e22, _e21);
			m128[2] = _mm_set_ps(_e34, _e33, _e32, _e31);
			m128[3] = _mm_set_ps(_e44, _e43, _e42, _e41);
		}
		void operator=(const mat4f& _matrix) {
			m128[0] = _matrix.m128[0];
			m128[1] = _matrix.m128[1];
			m128[2] = _matrix.m128[2];
			m128[3] = _matrix.m128[3];
		}
		void operator=(const float* _matrixFP) {
			m128[0] = _mm_load_ps(_matrixFP);
			m128[1] = _mm_load_ps(&_matrixFP[4]);
			m128[2] = _mm_load_ps(&_matrixFP[8]);
			m128[3] = _mm_load_ps(&_matrixFP[12]);
		}
		void Set(const float& _diagonal) {
			m128[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, _diagonal);
			m128[1] = _mm_set_ps(0.0f, 0.0f, _diagonal, 0.0f);
			m128[2] = _mm_set_ps(0.0f, _diagonal, 0.0f, 0.0f);
			m128[3] = _mm_set_ps(_diagonal, 0.0f, 0.0f, 0.0f);
		}
		void Set(const __m128& _vectorSSEx, const __m128& _vectorSSEy, const __m128& _vectorSSEz, const __m128& _vectorSSEt) {
			m128[0] = _vectorSSEx;
			m128[1] = _vectorSSEy;
			m128[2] = _vectorSSEz;
			m128[3] = _vectorSSEt;
		}
		void Set(const float* _vectorFPx, const float* _vectorFPy, const float* _vectorFPz, const float* _vectorFPt) {
			m128[0] = _mm_load_ps(_vectorFPx);
			m128[1] = _mm_load_ps(_vectorFPy);
			m128[2] = _mm_load_ps(_vectorFPz);
			m128[3] = _mm_load_ps(_vectorFPt);
		}
		void Set(const float* _matrixFP) {
			m128[0] = _mm_load_ps(_matrixFP);
			m128[1] = _mm_load_ps(&_matrixFP[4]);
			m128[2] = _mm_load_ps(&_matrixFP[8]);
			m128[3] = _mm_load_ps(&_matrixFP[12]);
		}
		void Set(const float& _e11, const float& _e12, const float& _e13, const float& _e14,
			const float& _e21, const float& _e22, const float& _e23, const float& _e24,
			const float& _e31, const float& _e32, const float& _e33, const float& _e34,
			const float& _e41, const float& _e42, const float& _e43, const float& _e44) {
			m128[0] = _mm_set_ps(_e14, _e13, _e12, _e11);
			m128[1] = _mm_set_ps(_e24, _e23, _e22, _e21);
			m128[2] = _mm_set_ps(_e34, _e33, _e32, _e31);
			m128[3] = _mm_set_ps(_e44, _e43, _e42, _e41);
		}

		//Absolute Value
		void Abs() {
			m128[0] = sml::VectorAbs(m128[0]);
			m128[1] = sml::VectorAbs(m128[1]);
			m128[2] = sml::VectorAbs(m128[2]);
			m128[3] = sml::VectorAbs(m128[3]);
		}

		//Equality Check
		bool IsZero() const {
			return sml::VectorIsZero(m128[0]) & sml::VectorIsZero(m128[1]) & sml::VectorIsZero(m128[2]) & sml::VectorIsZero(m128[3]);
		}
		bool IsEqual(const mat4f& _matrix) const {
			return sml::VectorIsEqual(m128[0], _matrix.m128[0]) & sml::VectorIsEqual(m128[1], _matrix.m128[1]) & sml::VectorIsEqual(m128[2], _matrix.m128[2]) & sml::VectorIsEqual(m128[3], _matrix.m128[3]);
		}
		bool IsEqual(const float* _matrixFP) const {
			return sml::VectorIsEqual(m128[0], _matrixFP) & sml::VectorIsEqual(m128[1], &_matrixFP[4]) & sml::VectorIsEqual(m128[2], &_matrixFP[8]) & sml::VectorIsEqual(m128[3], &_matrixFP[12]);
		}

		//Addition
		void Add(const mat4f& _matrix) {
			m128[0] = _mm_add_ps(m128[0], _matrix.m128[0]);
			m128[1] = _mm_add_ps(m128[1], _matrix.m128[1]);
			m128[2] = _mm_add_ps(m128[2], _matrix.m128[2]);
			m128[3] = _mm_add_ps(m128[3], _matrix.m128[3]);
		}
		void Add(const float* _matrixFP) {
			m128[0] = _mm_add_ps(m128[0], _mm_load_ps(_matrixFP));
			m128[1] = _mm_add_ps(m128[1], _mm_load_ps(&_matrixFP[4]));
			m128[2] = _mm_add_ps(m128[2], _mm_load_ps(&_matrixFP[8]));
			m128[3] = _mm_add_ps(m128[3], _mm_load_ps(&_matrixFP[12]));
		}
		void operator+=(const mat4f& _matrix) {
			Add(_matrix);
		}
		void operator+=(const float* _matrixFP) {
			Add(_matrixFP);
		}
		friend mat4f operator+(const mat4f& _matrix1, const mat4f& _matrix2) {
			return mat4f(
				_mm_add_ps(_matrix1.m128[0], _matrix2.m128[0]),
				_mm_add_ps(_matrix1.m128[1], _matrix2.m128[1]),
				_mm_add_ps(_matrix1.m128[2], _matrix2.m128[2]),
				_mm_add_ps(_matrix1.m128[3], _matrix2.m128[3])
			);
		}
		friend mat4f operator+(const mat4f& _matrix, const float* _matrixFP) {
			return mat4f(
				_mm_add_ps(_matrix.m128[0], _mm_load_ps(_matrixFP)),
				_mm_add_ps(_matrix.m128[1], _mm_load_ps(&_matrixFP[4])),
				_mm_add_ps(_matrix.m128[2], _mm_load_ps(&_matrixFP[8])),
				_mm_add_ps(_matrix.m128[3], _mm_load_ps(&_matrixFP[12]))
			);
		}
		friend mat4f operator+(const float* _matrixFP, const mat4f& _matrix) {
			return mat4f(
				_mm_add_ps(_mm_load_ps(_matrixFP), _matrix.m128[0]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[4]), _matrix.m128[1]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[8]), _matrix.m128[2]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[12]), _matrix.m128[3])
			);
		}

		//Subtraction
		void Sub(const mat4f& _matrix) {
			m128[0] = _mm_sub_ps(m128[0], _matrix.m128[0]);
			m128[1] = _mm_sub_ps(m128[1], _matrix.m128[1]);
			m128[2] = _mm_sub_ps(m128[2], _matrix.m128[2]);
			m128[3] = _mm_sub_ps(m128[3], _matrix.m128[3]);
		}
		void Sub(const float* _matrixFP) {
			m128[0] = _mm_sub_ps(m128[0], _mm_load_ps(_matrixFP));
			m128[1] = _mm_sub_ps(m128[1], _mm_load_ps(&_matrixFP[4]));
			m128[2] = _mm_sub_ps(m128[2], _mm_load_ps(&_matrixFP[8]));
			m128[3] = _mm_sub_ps(m128[3], _mm_load_ps(&_matrixFP[12]));
		}
		void operator-=(const mat4f& _matrix) {
			Sub(_matrix);
		}
		void operator-=(const float* _matrixFP) {
			Sub(_matrixFP);
		}
		friend mat4f operator-(const mat4f& _matrix1, const mat4f& _matrix2) {
			return mat4f(
				_mm_sub_ps(_matrix1.m128[0], _matrix2.m128[0]),
				_mm_sub_ps(_matrix1.m128[1], _matrix2.m128[1]),
				_mm_sub_ps(_matrix1.m128[2], _matrix2.m128[2]),
				_mm_sub_ps(_matrix1.m128[3], _matrix2.m128[3])
			);
		}
		friend mat4f operator-(const mat4f& _matrix, const float* _matrixFP) {
			return mat4f(
				_mm_sub_ps(_matrix.m128[0], _mm_load_ps(_matrixFP)),
				_mm_sub_ps(_matrix.m128[1], _mm_load_ps(&_matrixFP[4])),
				_mm_sub_ps(_matrix.m128[2], _mm_load_ps(&_matrixFP[8])),
				_mm_sub_ps(_matrix.m128[3], _mm_load_ps(&_matrixFP[12]))
			);
		}
		friend mat4f operator-(const float* _matrixFP, const mat4f& _matrix) {
			return mat4f(
				_mm_sub_ps(_mm_load_ps(_matrixFP), _matrix.m128[0]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[4]), _matrix.m128[1]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[8]), _matrix.m128[2]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[12]), _matrix.m128[3])
			);
		}

		//Matrix Scalar Division
		void Div(const float& _scalar) {
			m128[0] = _mm_div_ps(m128[0], _mm_set1_ps(_scalar));
			m128[1] = _mm_div_ps(m128[1], _mm_set1_ps(_scalar));
			m128[2] = _mm_div_ps(m128[2], _mm_set1_ps(_scalar));
			m128[3] = _mm_div_ps(m128[3], _mm_set1_ps(_scalar));
		}
		void operator/=(const float& _scalar) {
			m128[0] = _mm_div_ps(m128[0], _mm_set1_ps(_scalar));
			m128[1] = _mm_div_ps(m128[1], _mm_set1_ps(_scalar));
			m128[2] = _mm_div_ps(m128[2], _mm_set1_ps(_scalar));
			m128[3] = _mm_div_ps(m128[3], _mm_set1_ps(_scalar));
		}
		friend mat4f operator/(const mat4f& _matrix, const float& _scalar) {
			return mat4f(
			_mm_div_ps(_matrix.m128[0], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[1], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[2], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[3], _mm_set1_ps(_scalar))
			);
		}

		//Matrix Scalar Multiplication
		void MulS(const float& _scalar) {
			m128[0] = _mm_mul_ps(m128[0], _mm_set1_ps(_scalar));
			m128[1] = _mm_mul_ps(m128[1], _mm_set1_ps(_scalar));
			m128[2] = _mm_mul_ps(m128[2], _mm_set1_ps(_scalar));
			m128[3] = _mm_mul_ps(m128[3], _mm_set1_ps(_scalar));
		}
		void operator*=(const float& _scalar) {
			MulS(_scalar);
		}
		friend mat4f operator*(const mat4f& _matrix, const float& _scalar) {
			return mat4f(
				_mm_mul_ps(_matrix.m128[0], _mm_set1_ps(_scalar)),
				_mm_mul_ps(_matrix.m128[1], _mm_set1_ps(_scalar)),
				_mm_mul_ps(_matrix.m128[2], _mm_set1_ps(_scalar)),
				_mm_mul_ps(_matrix.m128[3], _mm_set1_ps(_scalar))
			);
		}
		friend mat4f operator*(const float& _scalar, const mat4f& _matrix) {
			return mat4f(
				_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[0]),
				_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[1]),
				_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[2]),
				_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[3])
			);
		}

		//Matrix-Matrix Multiplication
		void MulM(const mat4f& _matrix) {
			*this = mat4f(
				Dot(m128[0], _matrix.m128[0]),
				Dot(m128[0], _matrix.m128[1]),
				Dot(m128[0], _matrix.m128[2]),
				Dot(m128[0], _matrix.m128[3]),

				Dot(m128[1], _matrix.m128[0]),
				Dot(m128[1], _matrix.m128[1]),
				Dot(m128[1], _matrix.m128[2]),
				Dot(m128[1], _matrix.m128[3]),

				Dot(m128[2], _matrix.m128[0]),
				Dot(m128[2], _matrix.m128[1]),
				Dot(m128[2], _matrix.m128[2]),
				Dot(m128[2], _matrix.m128[3]),

				Dot(m128[3], _matrix.m128[0]),
				Dot(m128[3], _matrix.m128[1]),
				Dot(m128[3], _matrix.m128[2]),
				Dot(m128[3], _matrix.m128[3])
			);
		}
		void MulM(const float* _matrixFP) {
			mat4f _matrix(_matrixFP);
			*this = mat4f(
				Dot(m128[0], _matrix.m128[0]),
				Dot(m128[0], _matrix.m128[1]),
				Dot(m128[0], _matrix.m128[2]),
				Dot(m128[0], _matrix.m128[3]),

				Dot(m128[1], _matrix.m128[0]),
				Dot(m128[1], _matrix.m128[1]),
				Dot(m128[1], _matrix.m128[2]),
				Dot(m128[1], _matrix.m128[3]),

				Dot(m128[2], _matrix.m128[0]),
				Dot(m128[2], _matrix.m128[1]),
				Dot(m128[2], _matrix.m128[2]),
				Dot(m128[2], _matrix.m128[3]),

				Dot(m128[3], _matrix.m128[0]),
				Dot(m128[3], _matrix.m128[1]),
				Dot(m128[3], _matrix.m128[2]),
				Dot(m128[3], _matrix.m128[3])
			);
		}
		void operator*=(const mat4f& _matrix) {
			MulM(_matrix);
		}
		void operator*=(const float* _matrixFP) {
			MulM(_matrixFP);
		}
		friend mat4f operator*(const mat4f& _matrix1, const mat4f& _matrix2) {
			return mat4f(
				Dot(_matrix1.m128[0], _matrix2.m128[0]),
				Dot(_matrix1.m128[0], _matrix2.m128[1]),
				Dot(_matrix1.m128[0], _matrix2.m128[2]),
				Dot(_matrix1.m128[0], _matrix2.m128[3]),

				Dot(_matrix1.m128[1], _matrix2.m128[0]),
				Dot(_matrix1.m128[1], _matrix2.m128[1]),
				Dot(_matrix1.m128[1], _matrix2.m128[2]),
				Dot(_matrix1.m128[1], _matrix2.m128[3]),

				Dot(_matrix1.m128[2], _matrix2.m128[0]),
				Dot(_matrix1.m128[2], _matrix2.m128[1]),
				Dot(_matrix1.m128[2], _matrix2.m128[2]),
				Dot(_matrix1.m128[2], _matrix2.m128[3]),

				Dot(_matrix1.m128[3], _matrix2.m128[0]),
				Dot(_matrix1.m128[3], _matrix2.m128[1]),
				Dot(_matrix1.m128[3], _matrix2.m128[2]),
				Dot(_matrix1.m128[3], _matrix2.m128[3])
			);
		}
		friend mat4f operator*(const mat4f& _matrix, const float* _matrixFP) {
			mat4f matrixFP(_matrixFP);
			return mat4f(
				Dot(_matrix.m128[0], matrixFP.m128[0]),
				Dot(_matrix.m128[0], matrixFP.m128[1]),
				Dot(_matrix.m128[0], matrixFP.m128[2]),
				Dot(_matrix.m128[0], matrixFP.m128[3]),

				Dot(_matrix.m128[1], matrixFP.m128[0]),
				Dot(_matrix.m128[1], matrixFP.m128[1]),
				Dot(_matrix.m128[1], matrixFP.m128[2]),
				Dot(_matrix.m128[1], matrixFP.m128[3]),

				Dot(_matrix.m128[2], matrixFP.m128[0]),
				Dot(_matrix.m128[2], matrixFP.m128[1]),
				Dot(_matrix.m128[2], matrixFP.m128[2]),
				Dot(_matrix.m128[2], matrixFP.m128[3]),

				Dot(_matrix.m128[3], matrixFP.m128[0]),
				Dot(_matrix.m128[3], matrixFP.m128[1]),
				Dot(_matrix.m128[3], matrixFP.m128[2]),
				Dot(_matrix.m128[3], matrixFP.m128[3])
			);
		}
		friend mat4f operator*(const float* _matrixFP, const mat4f& _matrix) {
			mat4f matrixFP(_matrixFP);
			return mat4f(
				Dot(matrixFP.m128[0], _matrix.m128[0]),
				Dot(matrixFP.m128[0], _matrix.m128[1]),
				Dot(matrixFP.m128[0], _matrix.m128[2]),
				Dot(matrixFP.m128[0], _matrix.m128[3]),

				Dot(matrixFP.m128[1], _matrix.m128[0]),
				Dot(matrixFP.m128[1], _matrix.m128[1]),
				Dot(matrixFP.m128[1], _matrix.m128[2]),
				Dot(matrixFP.m128[1], _matrix.m128[3]),

				Dot(matrixFP.m128[2], _matrix.m128[0]),
				Dot(matrixFP.m128[2], _matrix.m128[1]),
				Dot(matrixFP.m128[2], _matrix.m128[2]),
				Dot(matrixFP.m128[2], _matrix.m128[3]),

				Dot(matrixFP.m128[3], _matrix.m128[0]),
				Dot(matrixFP.m128[3], _matrix.m128[1]),
				Dot(matrixFP.m128[3], _matrix.m128[2]),
				Dot(matrixFP.m128[3], _matrix.m128[3])
			);
		}

		//Matrix-Vector Multiplication
		__m128 MulMV(const __m128& _vectorSSE) {
			return _mm_set_ps(
				sml::Dot(m128[0], _vectorSSE),
				sml::Dot(m128[1], _vectorSSE),
				sml::Dot(m128[2], _vectorSSE),
				sml::Dot(m128[3], _vectorSSE)
			);
		}
		__m128 MulMV(const float* _vectorFP) {
			__m128 v = _mm_load_ps(_vectorFP);
			return _mm_set_ps(
				sml::Dot(m128[0], v),
				sml::Dot(m128[1], v),
				sml::Dot(m128[2], v),
				sml::Dot(m128[3], v)
			);
		}
		friend __m128 operator*(const mat4f& _matrix, const __m128& _vectorSSE) {
			return _mm_set_ps(
				sml::Dot(_matrix.m128[0], _vectorSSE),
				sml::Dot(_matrix.m128[1], _vectorSSE),
				sml::Dot(_matrix.m128[2], _vectorSSE),
				sml::Dot(_matrix.m128[3], _vectorSSE)
			);
		}

		//Vector-Matrix Multiplication
		__m128 MulVM(const __m128& _vectorSSE) {
			mat4f _matrixT = ~(*this);
			return _mm_set_ps(
				sml::Dot(_vectorSSE, _matrixT.m128[0]),
				sml::Dot(_vectorSSE, _matrixT.m128[1]),
				sml::Dot(_vectorSSE, _matrixT.m128[2]),
				sml::Dot(_vectorSSE, _matrixT.m128[3])
			);
		}
		__m128 MulVM(const float* _vectorFP) {
			mat4f _matrixT = ~(*this);
			__m128 v = _mm_load_ps(_vectorFP);
			return _mm_set_ps(
				sml::Dot(v, _matrixT.m128[0]),
				sml::Dot(v, _matrixT.m128[1]),
				sml::Dot(v, _matrixT.m128[2]),
				sml::Dot(v, _matrixT.m128[3])
			);
		}
		friend __m128 operator*(const __m128& _vectorSSE, const mat4f& _matrix) {
			mat4f _matrixT = ~_matrix;
			return _mm_set_ps(
				sml::Dot(_vectorSSE, _matrixT.m128[0]),
				sml::Dot(_vectorSSE, _matrixT.m128[1]),
				sml::Dot(_vectorSSE, _matrixT.m128[2]),
				sml::Dot(_vectorSSE, _matrixT.m128[3])
			);
		}

		// Negate
		void Negate() {
			m128[0] = _mm_xor_ps(m128[0], SSE_NEG_ZERO);
			m128[1] = _mm_xor_ps(m128[1], SSE_NEG_ZERO);
			m128[2] = _mm_xor_ps(m128[2], SSE_NEG_ZERO);
			m128[3] = _mm_xor_ps(m128[3], SSE_NEG_ZERO);
		}
		mat4f operator-() const {
			return mat4f(
				_mm_xor_ps(m128[0], SSE_NEG_ZERO),
				_mm_xor_ps(m128[1], SSE_NEG_ZERO),
				_mm_xor_ps(m128[2], SSE_NEG_ZERO),
				_mm_xor_ps(m128[3], SSE_NEG_ZERO)
			);
		}

		//Translation
		void Translate(const float& _x, const float& _y, const float& _z, const float& _w = 1.0f) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
				_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
				_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
				_mm_set_ps( _w ,  _z ,  _y ,  _x )
			) * (*this);
		}
		void Translate(const __m128& _vectorSSE) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
				_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
				_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
				_vectorSSE
			) * (*this);
		}
		void Translate(const float* _vectorFP) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
				_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
				_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
				_mm_load_ps(_vectorFP)
			) * (*this);
		}

		//Scaling
		void Scale(const float& _diagonal) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f, _diagonal),
				_mm_set_ps(0.0f, 0.0f, _diagonal, 0.0f),
				_mm_set_ps(0.0f, _diagonal, 0.0f, 0.0f),
				_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
			) * (*this);
		}
		void Scale(const float& _x, const float& _y, const float& _z, const float& _w = 1.0f) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f,  _x ),
				_mm_set_ps(0.0f, 0.0f,  _y , 0.0f),
				_mm_set_ps(0.0f,  _z , 0.0f, 0.0f),
				_mm_set_ps( _w,  0.0f, 0.0f, 0.0f)
			) * (*this);
		}
		void Scale(const __m128& _vectorSSE) {
			//TODO: Research a better way to do this!
			*this = mat4f(
				_mm_mul_ps(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f), _vectorSSE),
				_mm_mul_ps(_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f), _vectorSSE),
				_mm_mul_ps(_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f), _vectorSSE),
				_mm_mul_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), _vectorSSE)
			) * (*this);
		}
		void Scale(const float* _vectorFP) {
			*this = mat4f(
				_mm_set_ps(0.0f, 0.0f, 0.0f, _vectorFP[0]),
				_mm_set_ps(0.0f, 0.0f, _vectorFP[1], 0.0f),
				_mm_set_ps(0.0f, _vectorFP[2], 0.0f, 0.0f),
				_mm_set_ps(_vectorFP[3], 0.0f, 0.0f, 0.0f)
			) * (*this);
		}

//		//Rotation
//		void Rotate(const float& _radians, const float& _x, const float& _y, const float& _z) {
//			m128[0] = m128[1] = m128[2] = m128[3] = _mm_set1_ps(1337.0f);
//		}
//		void Rotate(const float& _radians, const __m128& _vectorSSE) {
//			m128[0] = m128[1] = m128[2] = m128[3] = _mm_set1_ps(1337.0f);
//		}
//		void Rotate(const float& _radians, const float* _vectorFP) {
//			m128[0] = m128[1] = m128[2] = m128[3] = _mm_set1_ps(1337.0f);
//		}

		//Axis Rotation
		void RotateX(const float& _radians) {
			float cosV = cosf(_radians);
			float sinV = sinf(_radians);

			*this = mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
			_mm_set_ps(0.0f,-sinV, cosV, 0.0f),
			_mm_set_ps(0.0f, cosV, sinV, 0.0f),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
			) * (*this);
		}
		void RotateY(const float& _radians) {
			float cosV = cosf(_radians);
			float sinV = sinf(_radians);

			*this = mat4f(
			_mm_set_ps(0.0f, sinV, 0.0f, cosV),
			_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
			_mm_set_ps(0.0f, cosV, 0.0f,-sinV),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
			) * (*this);
		}
		void RotateZ(const float& _radians) {
			float cosV = cosf(_radians);
			float sinV = sinf(_radians);

			*this = mat4f(
			_mm_set_ps(0.0f, 0.0f,-sinV, cosV),
			_mm_set_ps(0.0f, 0.0f, cosV, sinV),
			_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
			) * (*this);
		}

		//Transpose
		void Transpose() {
			_MM_TRANSPOSE4_PS(m128[0], m128[1], m128[2], m128[3]);
		}
		mat4f operator~() const  {
			__m128 t0 = _mm_shuffle_ps((m128[0]), (m128[1]), 0x44);
			__m128 t2 = _mm_shuffle_ps((m128[0]), (m128[1]), 0xEE);
			__m128 t1 = _mm_shuffle_ps((m128[2]), (m128[3]), 0x44);
			__m128 t3 = _mm_shuffle_ps((m128[2]), (m128[3]), 0xEE);

			return mat4f(
			_mm_shuffle_ps(t0, t1, 0x88),
			_mm_shuffle_ps(t0, t1, 0xDD),
			_mm_shuffle_ps(t2, t3, 0x88),
			_mm_shuffle_ps(t2, t3, 0xDD)
			);
		}

		//Determinant
		float Determinant() const {
			//TODO: A bit more research for the 4x4 determinant. This is a bunch of shuffles. It works though.
			return M128AddComponents(_mm_mul_ps(m128[0], _mm_set_ps(
				-Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
				-Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 2, 1)))
			)));
		}

		//Invserse
		void Inverse() {
			//Determinant
			float det = Determinant();
			if (det < FLT_EPSILON)
				return;

			//Transpose Determinant Matrix
			Transpose();
			*this = (
				 Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 2, 1))),

				-Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 3, 2, 0))),

				 Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 3, 1, 0))),

				-Determinant3D(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
				-Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
				 Determinant3D(_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(3, 2, 1, 0)))
			);

			return MulS(1.0f / det);
		}

	private:
		float Determinant3D(__m128 a, __m128 b, __m128 c) const {
			return sml::Dot(a, Cross(b, c));
		}
	};

	//Matrix Absolute Value
	static mat4f MatrixAbs(const mat4f& _matrix) {
			return mat4f(
				sml::VectorAbs(_matrix.m128[0]),
				sml::VectorAbs(_matrix.m128[1]),
				sml::VectorAbs(_matrix.m128[2]),
				sml::VectorAbs(_matrix.m128[3])
			);
	}
	static mat4f MatrixAbs(const float* _matrixFP) {
			return mat4f(
				sml::VectorAbs(_matrixFP),
				sml::VectorAbs(&_matrixFP[4]),
				sml::VectorAbs(&_matrixFP[8]),
				sml::VectorAbs(&_matrixFP[12])
			);
	}

	//Set
	static mat4f MatrixSet(const float& _diagonal) {
			return mat4f(_diagonal);
	}
	static mat4f MatrixSet(const __m128& _vectorSSEx, const __m128& _vectorSSEy, const __m128& _vectorSSEz, const __m128& _vectorSSEt) {
			return mat4f(_vectorSSEx, _vectorSSEy, _vectorSSEz, _vectorSSEt);
	}
	static mat4f MatrixSet(const float* _vectorFPx, const float* _vectorFPy, const float* _vectorFPz, const float* _vectorFPt) {
			return mat4f(_vectorFPx, _vectorFPy, _vectorFPz, _vectorFPt);
	}
	static mat4f MatrixSet(const float* _matrixFP) {
			return mat4f(_matrixFP);
	}
	static mat4f MatrixSet(const float& _e11, const float& _e12, const float& _e13, const float& _e14,
		const float& _e21, const float& _e22, const float& _e23, const float& _e24,
		const float& _e31, const float& _e32, const float& _e33, const float& _e34,
		const float& _e41, const float& _e42, const float& _e43, const float& _e44) {
			return mat4f(_e11, _e12, _e13, _e14,
						 _e21, _e22, _e23, _e24, 
						 _e31, _e32, _e33, _e34, 
						 _e41, _e42, _e43, _e44);
	}

	//Get Common Matrices
	static mat4f MatrixIdentity() {
			return mat4f(1337.0f);
	}
	static mat4f MatrixZero() {
			return mat4f(1337.0f);
	}

	//Transpose Matrix
	static mat4f Transpose(const mat4f& _matrix) {
		__m128 t0 = _mm_shuffle_ps((_matrix.m128[0]), (_matrix.m128[1]), 0x44);
		__m128 t2 = _mm_shuffle_ps((_matrix.m128[0]), (_matrix.m128[1]), 0xEE);
		__m128 t1 = _mm_shuffle_ps((_matrix.m128[2]), (_matrix.m128[3]), 0x44);
		__m128 t3 = _mm_shuffle_ps((_matrix.m128[2]), (_matrix.m128[3]), 0xEE);

		return mat4f(
			_mm_shuffle_ps(t0, t1, 0x88),
			_mm_shuffle_ps(t0, t1, 0xDD),
			_mm_shuffle_ps(t2, t3, 0x88),
			_mm_shuffle_ps(t2, t3, 0xDD)
		);
	}
	static mat4f Transpose(const float* _matrixFP) {
		mat4f ret(_matrixFP);
		_MM_TRANSPOSE4_PS(ret.m128[0], ret.m128[1], ret.m128[2], ret.m128[3]);
		return ret;
	}

	//Equality Check
	static bool MatrixIsZero(const mat4f& _matrix) {
		return sml::VectorIsZero(_matrix.m128[0]) & sml::VectorIsZero(_matrix.m128[1]) & sml::VectorIsZero(_matrix.m128[2]) & sml::VectorIsZero(_matrix.m128[3]);
	}
	static bool MatrixIsZero(const float* _matrixFP) {
		return sml::VectorIsZero(_mm_load_ps(_matrixFP)) & sml::VectorIsZero(_mm_load_ps(&_matrixFP[4])) & sml::VectorIsZero(_mm_load_ps(&_matrixFP[8])) & sml::VectorIsZero(_mm_load_ps(&_matrixFP[12]));
	}
	static bool MatrixIsEqual(const mat4f& _matrix1, const mat4f& _matrix2) {
		return sml::VectorIsEqual(_matrix1.m128[0], _matrix2.m128[0]) & sml::VectorIsEqual(_matrix1.m128[1], _matrix2.m128[1]) & sml::VectorIsEqual(_matrix1.m128[2], _matrix2.m128[2]) & sml::VectorIsEqual(_matrix1.m128[3], _matrix2.m128[3]);
	}
	static bool MatrixIsEqual(const mat4f& _matrix, const float* _matrixFP) {
		return sml::VectorIsEqual(_matrix.m128[0], _matrixFP) & sml::VectorIsEqual(_matrix.m128[1], &_matrixFP[4]) & sml::VectorIsEqual(_matrix.m128[2], &_matrixFP[8]) & sml::VectorIsEqual(_matrix.m128[3], &_matrixFP[12]);
	}
	static bool MatrixIsEqual(const float* _matrixFP, const mat4f& _matrix) {
		return sml::VectorIsEqual(_matrixFP, _matrix.m128[0]) & sml::VectorIsEqual(&_matrixFP[4], _matrix.m128[1]) & sml::VectorIsEqual(&_matrixFP[8], _matrix.m128[2]) & sml::VectorIsEqual(&_matrixFP[12], _matrix.m128[3]);
	}
	static bool MatrixIsEqual(const float* _matrixFP1, const float* _matrixFP2) {
		return sml::VectorIsEqual(_matrixFP1, _matrixFP2) & sml::VectorIsEqual(&_matrixFP1[4], &_matrixFP2[4]) & sml::VectorIsEqual(&_matrixFP1[8], &_matrixFP2[8]) & sml::VectorIsEqual(&_matrixFP1[12], &_matrixFP2[12]);
	}

	//Matrix-Matrix Addition
	static mat4f MatrixAdd(const mat4f& _matrix1, const mat4f& _matrix2) {
			return mat4f(
				_mm_add_ps(_matrix1.m128[0], _matrix2.m128[0]),
				_mm_add_ps(_matrix1.m128[1], _matrix2.m128[1]),
				_mm_add_ps(_matrix1.m128[2], _matrix2.m128[2]),
				_mm_add_ps(_matrix1.m128[3], _matrix2.m128[3])
			);
	}
	static mat4f MatrixAdd(const mat4f& _matrix, const float* _matrixFP) {
			return mat4f(
				_mm_add_ps(_matrix.m128[0], _mm_load_ps(_matrixFP)),
				_mm_add_ps(_matrix.m128[1], _mm_load_ps(&_matrixFP[4])),
				_mm_add_ps(_matrix.m128[2], _mm_load_ps(&_matrixFP[8])),
				_mm_add_ps(_matrix.m128[3], _mm_load_ps(&_matrixFP[12]))
			);
	}
	static mat4f MatrixAdd(const float* _matrixFP, const mat4f& _matrix) {
			return mat4f(
				_mm_add_ps(_mm_load_ps(_matrixFP), _matrix.m128[0]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[4]), _matrix.m128[1]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[8]), _matrix.m128[2]),
				_mm_add_ps(_mm_load_ps(&_matrixFP[12]), _matrix.m128[3])
			);
	}
	static mat4f MatrixAdd(const float* _matrixFP1, const float* _matrixFP2) {
			return mat4f(
				_mm_add_ps(_mm_load_ps(_matrixFP1) , _mm_load_ps(_matrixFP2) ),
				_mm_add_ps(_mm_load_ps(&_matrixFP1[4]) , _mm_load_ps(&_matrixFP2[4]) ),
				_mm_add_ps(_mm_load_ps(&_matrixFP1[8]) , _mm_load_ps(&_matrixFP2[8]) ),
				_mm_add_ps(_mm_load_ps(&_matrixFP1[12]), _mm_load_ps(&_matrixFP2[12]))
			);
	}

	//Matrix-Matrix Subtraction
	static mat4f MatrixSub(const mat4f& _matrix1, const mat4f& _matrix2) {
			return mat4f(
				_mm_sub_ps(_matrix1.m128[0], _matrix2.m128[0]),
				_mm_sub_ps(_matrix1.m128[1], _matrix2.m128[1]),
				_mm_sub_ps(_matrix1.m128[2], _matrix2.m128[2]),
				_mm_sub_ps(_matrix1.m128[3], _matrix2.m128[3])
			);
	}
	static mat4f MatrixSub(const mat4f& _matrix, const float* _matrixFP) {
			return mat4f(
				_mm_sub_ps(_matrix.m128[0], _mm_load_ps(_matrixFP)),
				_mm_sub_ps(_matrix.m128[1], _mm_load_ps(&_matrixFP[4])),
				_mm_sub_ps(_matrix.m128[2], _mm_load_ps(&_matrixFP[8])),
				_mm_sub_ps(_matrix.m128[3], _mm_load_ps(&_matrixFP[12]))
			);
	}
	static mat4f MatrixSub(const float* _matrixFP, const mat4f& _matrix) {
			return mat4f(
				_mm_sub_ps(_mm_load_ps(_matrixFP), _matrix.m128[0]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[4]), _matrix.m128[1]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[8]), _matrix.m128[2]),
				_mm_sub_ps(_mm_load_ps(&_matrixFP[12]), _matrix.m128[3])
			);
	}
	static mat4f MatrixSub(const float* _matrixFP1, const float* _matrixFP2) {
			return mat4f(
				_mm_sub_ps(_mm_load_ps(_matrixFP1) , _mm_load_ps(_matrixFP2) ),
				_mm_sub_ps(_mm_load_ps(&_matrixFP1[4]) , _mm_load_ps(&_matrixFP2[4]) ),
				_mm_sub_ps(_mm_load_ps(&_matrixFP1[8]) , _mm_load_ps(&_matrixFP2[8]) ),
				_mm_sub_ps(_mm_load_ps(&_matrixFP1[12]), _mm_load_ps(&_matrixFP2[12]))
			);
	}

	//Matrix-Scalar Divide
	static mat4f MatrixDiv(const mat4f& _matrix, const float& _scalar) {
		return mat4f(
			_mm_div_ps(_matrix.m128[0], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[1], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[2], _mm_set1_ps(_scalar)),
			_mm_div_ps(_matrix.m128[3], _mm_set1_ps(_scalar))
		);
	}
	static mat4f MatrixDiv(const float* _matrixFP, const float& _scalar) {
		return mat4f(
			_mm_div_ps(_mm_load_ps(_matrixFP), _mm_set1_ps(_scalar)),
			_mm_div_ps(_mm_load_ps(&_matrixFP[4]), _mm_set1_ps(_scalar)),
			_mm_div_ps(_mm_load_ps(&_matrixFP[8]), _mm_set1_ps(_scalar)),
			_mm_div_ps(_mm_load_ps(&_matrixFP[12]), _mm_set1_ps(_scalar))
		);
	}

	//Matrix-Scalar Multiply
	static mat4f MatrixMulS(const mat4f& _matrix, const float& _scalar) {
		return mat4f(
			_mm_mul_ps(_matrix.m128[0], _mm_set1_ps(_scalar)),
			_mm_mul_ps(_matrix.m128[1], _mm_set1_ps(_scalar)),
			_mm_mul_ps(_matrix.m128[2], _mm_set1_ps(_scalar)),
			_mm_mul_ps(_matrix.m128[3], _mm_set1_ps(_scalar))
		);
	}
	static mat4f MatrixMulS(const float& _scalar, const mat4f& _matrix) {
		return mat4f(
			_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[0]),
			_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[1]),
			_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[2]),
			_mm_mul_ps(_mm_set1_ps(_scalar), _matrix.m128[3])
		);
	}
	static mat4f MatrixMulS(const float* _matrixFP, const float& _scalar) {
		return mat4f(
			_mm_mul_ps(_mm_load_ps(_matrixFP), _mm_set1_ps(_scalar)),
			_mm_mul_ps(_mm_load_ps(&_matrixFP[4]), _mm_set1_ps(_scalar)),
			_mm_mul_ps(_mm_load_ps(&_matrixFP[8]), _mm_set1_ps(_scalar)),
			_mm_mul_ps(_mm_load_ps(&_matrixFP[12]), _mm_set1_ps(_scalar))
		);
	}
	static mat4f MatrixMulS(const float& _scalar, const float* _matrixFP) {
		return mat4f(
			_mm_mul_ps(_mm_set1_ps(_scalar), _mm_load_ps(_matrixFP)),
			_mm_mul_ps(_mm_set1_ps(_scalar), _mm_load_ps(&_matrixFP[4])),
			_mm_mul_ps(_mm_set1_ps(_scalar), _mm_load_ps(&_matrixFP[8])),
			_mm_mul_ps(_mm_set1_ps(_scalar), _mm_load_ps(&_matrixFP[12]))
		);
	}

	//Matrix-Matrix Multiply
	static mat4f MatrixMulM(const mat4f& _matrix1, const mat4f& _matrix2) {
		return mat4f(
			Dot(_matrix1.m128[0], _matrix2.m128[0]),
			Dot(_matrix1.m128[0], _matrix2.m128[1]),
			Dot(_matrix1.m128[0], _matrix2.m128[2]),
			Dot(_matrix1.m128[0], _matrix2.m128[3]),

			Dot(_matrix1.m128[1], _matrix2.m128[0]),
			Dot(_matrix1.m128[1], _matrix2.m128[1]),
			Dot(_matrix1.m128[1], _matrix2.m128[2]),
			Dot(_matrix1.m128[1], _matrix2.m128[3]),

			Dot(_matrix1.m128[2], _matrix2.m128[0]),
			Dot(_matrix1.m128[2], _matrix2.m128[1]),
			Dot(_matrix1.m128[2], _matrix2.m128[2]),
			Dot(_matrix1.m128[2], _matrix2.m128[3]),

			Dot(_matrix1.m128[3], _matrix2.m128[0]),
			Dot(_matrix1.m128[3], _matrix2.m128[1]),
			Dot(_matrix1.m128[3], _matrix2.m128[2]),
			Dot(_matrix1.m128[3], _matrix2.m128[3])
		);
	}
	static mat4f MatrixMulM(const mat4f& _matrix, const float* _matrixFP) {
		mat4f matrixFP(_matrixFP);
		return mat4f(
			Dot(_matrix.m128[0], matrixFP.m128[0]),
			Dot(_matrix.m128[0], matrixFP.m128[1]),
			Dot(_matrix.m128[0], matrixFP.m128[2]),
			Dot(_matrix.m128[0], matrixFP.m128[3]),

			Dot(_matrix.m128[1], matrixFP.m128[0]),
			Dot(_matrix.m128[1], matrixFP.m128[1]),
			Dot(_matrix.m128[1], matrixFP.m128[2]),
			Dot(_matrix.m128[1], matrixFP.m128[3]),

			Dot(_matrix.m128[2], matrixFP.m128[0]),
			Dot(_matrix.m128[2], matrixFP.m128[1]),
			Dot(_matrix.m128[2], matrixFP.m128[2]),
			Dot(_matrix.m128[2], matrixFP.m128[3]),

			Dot(_matrix.m128[3], matrixFP.m128[0]),
			Dot(_matrix.m128[3], matrixFP.m128[1]),
			Dot(_matrix.m128[3], matrixFP.m128[2]),
			Dot(_matrix.m128[3], matrixFP.m128[3])
		);
	}
	static mat4f MatrixMulM(const float* _matrixFP, const mat4f& _matrix) {
		mat4f matrixFP(_matrixFP);
		return mat4f(
			Dot(matrixFP.m128[0], _matrix.m128[0]),
			Dot(matrixFP.m128[0], _matrix.m128[1]),
			Dot(matrixFP.m128[0], _matrix.m128[2]),
			Dot(matrixFP.m128[0], _matrix.m128[3]),
			
			Dot(matrixFP.m128[1], _matrix.m128[0]),
			Dot(matrixFP.m128[1], _matrix.m128[1]),
			Dot(matrixFP.m128[1], _matrix.m128[2]),
			Dot(matrixFP.m128[1], _matrix.m128[3]),
			
			Dot(matrixFP.m128[2], _matrix.m128[0]),
			Dot(matrixFP.m128[2], _matrix.m128[1]),
			Dot(matrixFP.m128[2], _matrix.m128[2]),
			Dot(matrixFP.m128[2], _matrix.m128[3]),
			
			Dot(matrixFP.m128[3], _matrix.m128[0]),
			Dot(matrixFP.m128[3], _matrix.m128[1]),
			Dot(matrixFP.m128[3], _matrix.m128[2]),
			Dot(matrixFP.m128[3], _matrix.m128[3])
		);
	}
	static mat4f MatrixMulM(const float* _matrixFP1, const float* _matrixFP2) {
		mat4f matrixFP1(_matrixFP1);
		mat4f matrixFP2(_matrixFP2);
		return mat4f(
			Dot(matrixFP1.m128[0], matrixFP2.m128[0]),
			Dot(matrixFP1.m128[0], matrixFP2.m128[1]),
			Dot(matrixFP1.m128[0], matrixFP2.m128[2]),
			Dot(matrixFP1.m128[0], matrixFP2.m128[3]),

			Dot(matrixFP1.m128[1], matrixFP2.m128[0]),
			Dot(matrixFP1.m128[1], matrixFP2.m128[1]),
			Dot(matrixFP1.m128[1], matrixFP2.m128[2]),
			Dot(matrixFP1.m128[1], matrixFP2.m128[3]),

			Dot(matrixFP1.m128[2], matrixFP2.m128[0]),
			Dot(matrixFP1.m128[2], matrixFP2.m128[1]),
			Dot(matrixFP1.m128[2], matrixFP2.m128[2]),
			Dot(matrixFP1.m128[2], matrixFP2.m128[3]),

			Dot(matrixFP1.m128[3], matrixFP2.m128[0]),
			Dot(matrixFP1.m128[3], matrixFP2.m128[1]),
			Dot(matrixFP1.m128[3], matrixFP2.m128[2]),
			Dot(matrixFP1.m128[3], matrixFP2.m128[3])
		);
	}

	//Matrix-Vector Multiply
	static __m128 MatrixMulV(const mat4f& _matrix, const __m128& _vectorSSE) {
		return _mm_set_ps(
			sml::Dot(_matrix.m128[0], _vectorSSE), 
			sml::Dot(_matrix.m128[1], _vectorSSE), 
			sml::Dot(_matrix.m128[2], _vectorSSE), 
			sml::Dot(_matrix.m128[3], _vectorSSE)
		);
	}
	static __m128 MatrixMulV(const mat4f& _matrix, const float* _vectorFP) {
		__m128 v = _mm_load_ps(_vectorFP);
		return _mm_set_ps(
			sml::Dot(_matrix.m128[0], v),
			sml::Dot(_matrix.m128[1], v),
			sml::Dot(_matrix.m128[2], v),
			sml::Dot(_matrix.m128[3], v)
		);
	}
	static __m128 MatrixMulV(const float* _matrixFP, const __m128& _vectorSSE) {
		return _mm_set_ps(
			sml::Dot(_mm_load_ps(_matrixFP), _vectorSSE),
			sml::Dot(_mm_load_ps(&_matrixFP[4]), _vectorSSE),
			sml::Dot(_mm_load_ps(&_matrixFP[8]), _vectorSSE),
			sml::Dot(_mm_load_ps(&_matrixFP[12]), _vectorSSE)
		);
	}
	static __m128 MatrixMulV(const float* _matrixFP, const float* _vectorFP) {
		__m128 v = _mm_load_ps(_vectorFP);
		return _mm_set_ps(
			sml::Dot(_mm_load_ps(_matrixFP), v),
			sml::Dot(_mm_load_ps(&_matrixFP[4]), v),
			sml::Dot(_mm_load_ps(&_matrixFP[8]), v),
			sml::Dot(_mm_load_ps(&_matrixFP[12]), v)
		);
	}

	//Vector-Matrix Multiply
	static __m128 VectorMulM(const __m128& _vectorSSE, const mat4f& _matrix) {
		mat4f _matrixT = sml::Transpose(_matrix);
		return _mm_set_ps(
			sml::Dot(_vectorSSE, _matrixT.m128[0]),
			sml::Dot(_vectorSSE, _matrixT.m128[1]),
			sml::Dot(_vectorSSE, _matrixT.m128[2]),
			sml::Dot(_vectorSSE, _matrixT.m128[3])
		);
	}
	static __m128 VectorMulM(const __m128& _vectorSSE, const float* _matrixFP) {
		mat4f _matrixT = sml::Transpose(_matrixFP);
		return _mm_set_ps(
			sml::Dot(_vectorSSE, _matrixT.m128[0]),
			sml::Dot(_vectorSSE, _matrixT.m128[1]),
			sml::Dot(_vectorSSE, _matrixT.m128[2]),
			sml::Dot(_vectorSSE, _matrixT.m128[3])
		);
	}
	static __m128 VectorMulM(const float* _vectorFP, const mat4f& _matrix) {
		mat4f _matrixT = sml::Transpose(_matrix);
		__m128 v = _mm_load_ps(_vectorFP);
		return _mm_set_ps(
			sml::Dot(v, _matrixT.m128[0]),
			sml::Dot(v, _matrixT.m128[1]),
			sml::Dot(v, _matrixT.m128[2]),
			sml::Dot(v, _matrixT.m128[3])
		);
	}
	static __m128 VectorMulM(const float* _vectorFP, const float* _matrixFP) {
		mat4f _matrixT = sml::Transpose(_matrixFP);
		__m128 v = _mm_load_ps(_vectorFP);
		return _mm_set_ps(
			sml::Dot(v, _matrixT.m128[0]),
			sml::Dot(v, _matrixT.m128[1]),
			sml::Dot(v, _matrixT.m128[2]),
			sml::Dot(v, _matrixT.m128[3])
		);
	}

	//Matrix Negate
	static mat4f MatrixNegate(const mat4f& _matrix) {
		return mat4f(
			_mm_xor_ps(_matrix.m128[0], SSE_NEG_ZERO),
			_mm_xor_ps(_matrix.m128[1], SSE_NEG_ZERO),
			_mm_xor_ps(_matrix.m128[2], SSE_NEG_ZERO),
			_mm_xor_ps(_matrix.m128[3], SSE_NEG_ZERO)
		);
	}
	static mat4f MatrixNegate(const float* _matrixFP) {
		return mat4f(
			_mm_xor_ps(_mm_load_ps(_matrixFP), SSE_NEG_ZERO),
			_mm_xor_ps(_mm_load_ps(&_matrixFP[4]), SSE_NEG_ZERO),
			_mm_xor_ps(_mm_load_ps(&_matrixFP[8]), SSE_NEG_ZERO),
			_mm_xor_ps(_mm_load_ps(&_matrixFP[12]), SSE_NEG_ZERO)
		);
	}

	//Translation Matrix
	static mat4f TranslationMatrix(const float& _x, const float& _y, const float& _z, const float& _w = 1.0f) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
			_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
			_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
			_mm_set_ps( _w ,  _z ,  _y ,  _x )
		);
	}
	static mat4f TranslationMatrix(const __m128& _vectorSSE) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
			_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
			_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
			_vectorSSE
		);
	}
	static mat4f TranslationMatrix(const float* _vectorFP) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
			_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
			_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
			_mm_load_ps(_vectorFP)
		);
	}

	//Scaling Matrix
	static mat4f ScalingMatrix(const float& _diagonal) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, _diagonal),
			_mm_set_ps(0.0f, 0.0f, _diagonal, 0.0f),
			_mm_set_ps(0.0f, _diagonal, 0.0f, 0.0f),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)	
		);
	}
	static mat4f ScalingMatrix(const float& _x, const float& _y, const float& _z, const float& _w = 1.0f) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f,  _x ),
			_mm_set_ps(0.0f, 0.0f,  _y , 0.0f),
			_mm_set_ps(0.0f,  _z , 0.0f, 0.0f),
			_mm_set_ps( _w,  0.0f, 0.0f, 0.0f)
		);
	}
	static mat4f ScalingMatrix(const __m128& _vectorSSE) {
		//TODO: Research a better way to do this!
		return mat4f(
			_mm_mul_ps(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f), _vectorSSE),
			_mm_mul_ps(_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f), _vectorSSE),
			_mm_mul_ps(_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f), _vectorSSE),
			_mm_mul_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), _vectorSSE)
		);
	}
	static mat4f ScalingMatrix(const float* _vectorFP) {
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, _vectorFP[0]),
			_mm_set_ps(0.0f, 0.0f, _vectorFP[1], 0.0f),
			_mm_set_ps(0.0f, _vectorFP[2], 0.0f, 0.0f),
			_mm_set_ps(_vectorFP[3], 0.0f, 0.0f, 0.0f)
		);
	}

	//Rotation Matrix
//	static mat4f RotationMatrix(const float& _radians, const float& _x, const float& _y, const float& _z) {
//			return mat4f(1337.0f);
//	}
//	static mat4f RotationMatrix(const float& _radians, const __m128& _vectorSSE) {
//			return mat4f(1337.0f);
//	}
//	static mat4f RotationMatrix(const __m128& _vectorSSE, const float& _radians) {
//			return mat4f(1337.0f);
//	}
//	static mat4f RotationMatrix(const float* _vectorFP, const float& _radians) {
//			return mat4f(1337.0f);
//	}
//	static mat4f RotationMatrix(const float& _radians, const float* _vectorFP) {
//			return mat4f(1337.0f);
//	}

	//Axis Rotation Matrices
	static mat4f XRotationMatrix(const float& _radians) {
		float cosV = cosf(_radians);
		float sinV = sinf(_radians);
		return mat4f(
			_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
			_mm_set_ps(0.0f,-sinV, cosV, 0.0f),
			_mm_set_ps(0.0f, cosV, sinV, 0.0f),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
		);
	}
	static mat4f YRotationMatrix(const float& _radians) {
		float cosV = cosf(_radians);
		float sinV = sinf(_radians);
		return mat4f(
			_mm_set_ps(0.0f, sinV, 0.0f, cosV),
			_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
			_mm_set_ps(0.0f, cosV, 0.0f,-sinV),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
		);
	}
	static mat4f ZRotationMatrix(const float& _radians) {
		float cosV = cosf(_radians);
		float sinV = sinf(_radians);
		return mat4f(
			_mm_set_ps(0.0f, 0.0f,-sinV, cosV),
			_mm_set_ps(0.0f, 0.0f, cosV, sinV),
			_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
			_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
		);
	}

	//Matrix Determinants
	static float Determinant3D(const __m128& a, const __m128& _b, const __m128& _c) {
		return sml::Dot(a, sml::Cross(_b, _c));
	}
	static float Determinant(const mat4f& _matrix) {
		//TODO: A bit more research for the 4x4 determinant. This is a bunch of shuffles. It works though.
		return sml::M128AddComponents(_mm_mul_ps(_matrix.m128[0], _mm_set_ps(
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(0, 3, 2, 1)))
		)));
	}
	static float Determinant(const float* _matrixFP) {
		//TODO: A bit more research for the 4x4 determinant. This is a bunch of shuffles. It works though.
		mat4f _matrix(_matrixFP);
		return sml::M128AddComponents(_mm_mul_ps(_matrix.m128[0], _mm_set_ps(
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(0, 3, 2, 1)))
		)));
	}

	//Inverse
	static mat4f Inverse(const mat4f& _matrix) {
		//Determinant
		float det = Determinant(_matrix);
		if (det < FLT_EPSILON)
			return _matrix;

		//Transpose Determinant Matrix
		mat4f transpose = sml::Transpose(_matrix);
		mat4f a_transpose(
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(0, 3, 2, 1))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(0, 3, 2, 1)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(0, 3, 2, 1))),

			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(1, 3, 2, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(1, 3, 2, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(1, 3, 2, 0))),

			 Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(2, 3, 1, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(2, 3, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(2, 3, 1, 0))),

			-Determinant3D(_mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
			-Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[3], _matrix.m128[3], _MM_SHUFFLE(3, 2, 1, 0))),
			 Determinant3D(_mm_shuffle_ps(_matrix.m128[0], _matrix.m128[0], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[1], _matrix.m128[1], _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(_matrix.m128[2], _matrix.m128[2], _MM_SHUFFLE(3, 2, 1, 0)))
			);

		return sml::MatrixMulS(a_transpose, 1.0f / det);
	}
	static mat4f Inverse(const float* _matrixFP) {
			return Inverse(mat4f(_matrixFP));
	}
	static mat4f InverseFast(const mat4f& _matrix) {
			return mat4f(1337.0f);
	}

}

#ifdef MAT4_FLOATS_GLOBAL
typedef mat4f mat4;
#endif

#endif