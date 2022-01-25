#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;


int main(){



    float val = 254.0 / 2;

    int high = ceil(val);
    int low = floor(val);

    cout << low << " " << val << " " << high << endl;
}