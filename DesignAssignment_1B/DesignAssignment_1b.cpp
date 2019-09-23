//  DesignAssignment_1b
//  BRYSEN KOKUBUN
//  CPE-301
//  9/21/19

#include <iostream>
#include <cmath>

using namespace std;

int main()
{
    int div_5  = 0;
    int ndiv_5 = 0;
    int counter = 251;
    
    for (int i = 0; i < counter; i++)
    {
        if (i % 5 == 0)
        {
            div_5 += i;
        }
        else
        {
            ndiv_5 += i;
        }
    }
    
    cout << "The summation of values divisible by 5 from 1 to 250: "
         << div_5 << endl;
    
    cout << "The summation of values NOT divisible by 5 from 1 to 250: "
         << ndiv_5 << endl;
    
    return 0;
}
