
#include <iostream>

#include "Float.h"

int main()
{

    float a = 12;
    float b = -13;

    Float c(a);
    c = c + Float(b);

    std::cout << 
        a << " + " << 
        b << " = " << 
        a+b << " ?= " << 
        c.getfloat() << std::endl;


    std::cout <<
        Float(b) << std::endl <<
        Float(a) << std::endl <<
        c << std::endl <<
        Float(a+b) << std::endl;

    int incorrect = 0;

    if(false)
    for (int i = 1; i <= 12; i++) {
        for (int j = -1; j >= -13; j--) {
            a = (float) i;
            b = (float) j;
            c = Float(a) + Float(b);

            float r = a + b;

            if (c.getfloat() != r) {
                std::cout << std::endl<<
                    (float)i << " + " <<
                    (float)j << " != " <<
                    c.getfloat() << std::endl <<
                    r << std::endl <<
                    "incorrect :" <<
                    c <<std::endl <<
                    "correct   :" <<
                    Float(r) << std::endl;

                incorrect++;
                    
            }

        }
    }

    std::cout << std::endl << "incorrect : " << incorrect;
}
