#pragma once

#define SIGN_BIT_MASK 0x80000000        //corresponds to -0
#define EXPONENT_MASK 0x7f800000        //corresponds to inf
#define MANTISSA_MASK 0x007fffff        //corresponds to 1.17549421069e-38
#define EXPONENT_OFFSET 127
//exponent of 0x7f800000 with any non zero mantissa is NaN, sign bit can be anything
#define IMPLICIT_BIT 0x00800000

#include <cstdint>
#include <iostream>

class Float {

public:

	Float();
	Float(bool sign, unsigned int mantissa, unsigned int exponent);
	Float(float f);
	float getfloat();

	friend std::ostream& operator<<(std::ostream& os, const Float& number);


	Float operator*(Float b);
	Float operator/(Float b);
	Float operator-(Float b);
	Float operator+(Float b);

	Float sqrt();


private:
	uint32_t mantissa;
	uint8_t exponent;
	uint8_t sign;

	static void printBin32(uint32_t x) {
		for (int i = 31; i >= 0; i--) {
			std::cout << (x >> i & 0x1);
			if ((i % 8 == 0) && i != 0) std::cout << "_";
		}
	}

};




/*
float ksqrt(float r){

//holds initial guess to start
float result = 1;

int i;
for(i = 0; i < 6; i++){
	printf("(NM)result at iteration[%d] = %f\n", i, result);
	result = ( result * result + r) / ( 2 * result );
}

return result;
}


float ipow(float x, int n){
	float y, r;

	y = (n > 0) ? x : 1;
	r = 1;

	for(n; n > 1; n >>= 1){
		r *= ( (n & 1) == 1 ) ? y : 1;
		y *= y;
	}

	return y * r;
}
*/