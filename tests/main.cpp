#include <iostream>
#include "ad.hpp"
#include "et.hpp"

using namespace std;
using namespace et;
using namespace ad;

int main() {
  using basetype = ad::tangent<double>::type;
//  using basetype = ad::tangent<ad::tangent<double>::type>::type;
//  using type = basetype;
  using type = et::ExprLeaf<basetype>;
    


      type x = 2.0;
      type y = 4.0;
      type z = 5.0;
      type w =x*y+2-sin(z);



  //type a=0;
  //derivative(derivative(y)) = 0;

  //derivative(y) = 1;
  //cout << "dx="<< derivative(x) << endl;
//  cout << derivative(value(x)) << endl;
  //cout << "dy="<<derivative(y) << endl;
  cout << "w="<< w << endl;

  return 0;
}
