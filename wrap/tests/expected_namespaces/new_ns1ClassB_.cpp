// automatically generated by wrap
#include <wrap/matlab.h>
#include <path/to/ns1.h>
#include <path/to/ns1/ClassB.h>
void mexFunction(int nargout, mxArray *out[], int nargin, const mxArray *in[])
{
  checkArguments("new_ns1ClassB_",nargout,nargin,0);
  ns1::ClassB* self = new ns1::ClassB();
  out[0] = wrap_constructed(self,"ns1ClassB");
}
