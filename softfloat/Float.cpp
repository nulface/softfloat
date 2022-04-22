#include "Float.h"

Float::Float()
{
	mantissa = 0;
	exponent = 0;
	sign = false;
}

Float::Float(bool sign, unsigned int mantissa, unsigned int exponent)
{
	this->sign = sign;
	this->mantissa = mantissa;
	this->exponent = exponent;
}

Float::Float(float f)
{
	uint32_t x = *((uint32_t*)&f);

	this->mantissa = x & 0x007fffff;
	this->exponent = (x & 0x7f800000) >> 23;
	this->sign = (((x & 0x80000000) > 0) ? true : false);

}

std::ostream& operator<<(std::ostream& os, const Float& number) {

	os << ((number.sign == 0) ? "+ " : "- ");

	for (int i = 7; i >= 0; i--) {
		os << (number.exponent >> i & 0x1);
	}
	//os << number.exponent - EXPONENT_OFFSET << " ";

	os << " ";

	for (int i = 22; i >= 0; i--) {
		os << (number.mantissa >> i & 0x1);
		if ((i % 8 == 0) && i != 0) os << "_";
	}
	//os << number.mantissa;

	return os;
}

float Float::getfloat()
{
	uint32_t x = (this->mantissa | (this->exponent << 23) | ((this->sign == 1) ? 0x80000000 : 0));
	return *(float*)(&x);
}

Float Float::operator*(Float b)
{

	uint64_t result = (uint64_t)(this->mantissa | IMPLICIT_BIT) * (uint64_t)(b.mantissa | IMPLICIT_BIT);

	if (result & ((uint64_t)0x1 << 47)) {

		return	Float(	this->sign ^ b.sign, 
						(uint32_t)(result >> 24) & MANTISSA_MASK,
						this->exponent + b.exponent - EXPONENT_OFFSET + 1);
	}
	else {

		return Float(	this->sign ^ b.sign, 
						(uint32_t)(result >> 23) & MANTISSA_MASK,
						this->exponent + b.exponent - EXPONENT_OFFSET);
	}

}

Float Float::operator/(Float b)
{
	return Float(this->getfloat() / b.getfloat());
}

Float Float::operator-(Float b)
{
	b.sign = !b.sign;
	return *this + b;
}

Float Float::operator+(Float b)
{
	//return Float(this->getfloat() + b.getfloat());

	//check if both are equal
	//do this a different way...
	//if (this->mantissa == b.mantissa && this->exponent == b.exponent) {
	//	return Float(false, 0, 0);
	//}

	//check if one is zero
	if (this->mantissa == 0 && this->exponent == 0) {
		return b;
	}
	if (b.mantissa == 0 && b.exponent == 0) {
		return *this;
	}



	Float r;

	r.sign = 0;

	uint64_t aman, bman, rman;

	aman = this->mantissa	| IMPLICIT_BIT;
	bman = b.mantissa		| IMPLICIT_BIT;
	rman = 0;

	uint32_t exponent_difference = 0;
	uint32_t common_factor = 0;

	//a < b
	if (this->exponent < b.exponent) {
		common_factor = this->exponent;
		exponent_difference = b.exponent - this->exponent;
		bman <<= exponent_difference;
	}
	//a > b
	else{
		common_factor = b.exponent;
		exponent_difference = this->exponent - b.exponent;
		aman <<= exponent_difference;
	}


	//I am sure there is a better way to set up this logic.
	if (this->sign ^ b.sign) {
		//this handles -+ and +-

		//this should fix the issue with 10-9
		//add carry bit to both numbers
		aman |= 0x80000000;
		bman |= 0x80000000;

		//only one of these will be made twos compliment
		if (this->sign == 1) {
			//add a carry bit to the negative number
			//aman |= 0x80000000;
			aman = ~aman + 1;
		}
		if (b.sign == 1) { 
			//bman |= 0x80000000;
			bman = ~bman + 1; 
		}

		rman = aman + bman;

		//this means a negative result
		//if it doesnt overflow, that means we have a negative result
		//the highest bit will be set indicating that it is negative
		//so we do twos compliment on it
		if (rman & ((uint64_t)0x1 << 63)) {
			r.sign = 1;
			rman = ~rman + 1;
		}


	}
	else {

		//this case handles ++ and --

		r.sign = this->sign;
		rman = aman + bman;

	}

	if (rman == 0) {
		return Float(false, 0, 0);
	}

	/*
	two problems with this code: (that i am aware of)

	12 - 13 :
	//we need a carry bit

	10 - 9  :
	//where is the highest bit after subtraction???

	*/

	//WHY ARE THEY ALL 3 APART? WHAT THE FUCK

	int shamt = log2(rman) - 23;
	
	std::cout << "            ";
	printBin32(rman);
	
	std::cout << "\nshamt : " << shamt << std::endl;
	std::cout << "expdf : " << exponent_difference << std::endl;

	r.exponent = common_factor + shamt;
	rman >>= shamt;

	//r.exponent =  common_factor;
	//rman >>= 0;


	//if (rman & ((uint64_t)0x1 << (23 + exponent_difference + 1))) {
	//	r.exponent = exponent_difference + common_factor + 1;
	//	rman >>= (exponent_difference + 1);
	//
	//}
	//else if (rman & ((uint64_t)0x1 << (23 + exponent_difference))) {
	//	r.exponent = exponent_difference + common_factor;
	//	rman >>= exponent_difference;
	//}
	//else {
	//	int shamt = log2(rman) - 22;
	//	r.exponent = common_factor;
	//	//std::cout << "result : " << shamt << std::endl;
	//
	//	r.exponent += shamt;
	//	rman <<= -shamt;
	//}
	
	r.mantissa = rman & MANTISSA_MASK;

	return r;

}

Float Float::sqrt()
{
	//takes the square root of itself

    //initial guess.
    //there is a better way to find our initial result
    Float result(1.0f);

    //int i;
    //for(i = 0; i < 6; i++){
    //    //print_Float(result);
	//
    //    //printf("(SF)result at iteration[%d] = %f\n", i, extract_float(result));
	//
    //    result = fdiv(
    //                  fadd( fsqr(result), r ),
    //                  fmul( result, extract_struct( 2 ) )
    //                  );
    //}
    //result = extract_struct(1.0f);
    return result;
}




