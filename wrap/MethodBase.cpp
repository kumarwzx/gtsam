/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file MethodBase.ccp
 * @author Frank Dellaert
 * @author Andrew Melim
 * @author Richard Roberts
 **/

#include "Method.h"
#include "utilities.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>

using namespace std;
using namespace wrap;

/* ************************************************************************* */
void MethodBase::proxy_wrapper_fragments(FileWriter& proxyFile,
    FileWriter& wrapperFile, Str cppClassName, Str matlabQualName,
    Str matlabUniqueName, Str wrapperName,
    const TypeAttributesTable& typeAttributes,
    vector<string>& functionNames) const {

  // emit header, e.g., function varargout = templatedMethod(this, varargin)
  proxy_header(proxyFile);

  // Emit comments for documentation
  string up_name = boost::to_upper_copy(matlabName());
  proxyFile.oss << "      % " << up_name << " usage: ";
  usage_fragment(proxyFile, matlabName());

  // Emit URL to Doxygen page
  proxyFile.oss << "      % "
      << "Doxygen can be found at http://research.cc.gatech.edu/borg/sites/edu.borg/html/index.html"
      << endl;

  // Handle special case of single overload with all numeric arguments
  if (nrOverloads() == 1 && argumentList(0).allScalar()) {
    // Output proxy matlab code
    // TODO: document why is it OK to not check arguments in this case
    proxyFile.oss << "      ";
    const int id = (int) functionNames.size();
    emit_call(proxyFile, returnValue(0), wrapperName, id);

    // Output C++ wrapper code
    const string wrapFunctionName = wrapper_fragment(wrapperFile, cppClassName,
        matlabUniqueName, 0, id, typeAttributes);

    // Add to function list
    functionNames.push_back(wrapFunctionName);
  } else {
    // Check arguments for all overloads
    for (size_t i = 0; i < nrOverloads(); ++i) {

      // Output proxy matlab code
      proxyFile.oss << "      " << (i == 0 ? "" : "else");
      const int id = (int) functionNames.size();
      emit_conditional_call(proxyFile, returnValue(i), argumentList(i),
          wrapperName, id);

      // Output C++ wrapper code
      const string wrapFunctionName = wrapper_fragment(wrapperFile,
          cppClassName, matlabUniqueName, i, id, typeAttributes);

      // Add to function list
      functionNames.push_back(wrapFunctionName);
    }
    proxyFile.oss << "      else\n";
    proxyFile.oss
        << "        error('Arguments do not match any overload of function "
        << matlabQualName << "." << name_ << "');" << endl;
    proxyFile.oss << "      end\n";
  }

  proxyFile.oss << "    end\n";
}

/* ************************************************************************* */
string MethodBase::wrapper_fragment(FileWriter& wrapperFile, Str cppClassName,
    Str matlabUniqueName, int overload, int id,
    const TypeAttributesTable& typeAttributes) const {

  // generate code

  const string wrapFunctionName = matlabUniqueName + "_" + name_ + "_"
      + boost::lexical_cast<string>(id);

  const ArgumentList& args = argumentList(overload);
  const ReturnValue& returnVal = returnValue(overload);

  // call
  wrapperFile.oss << "void " << wrapFunctionName
      << "(int nargout, mxArray *out[], int nargin, const mxArray *in[])\n";
  // start
  wrapperFile.oss << "{\n";

  returnVal.wrapTypeUnwrap(wrapperFile);

  wrapperFile.oss << "  typedef boost::shared_ptr<" << cppClassName
      << "> Shared;" << endl;

  // get call
  // for static methods: cppClassName::staticMethod<TemplateVal>
  // for instance methods: obj->instanceMethod<TemplateVal>
  string expanded = wrapper_call(wrapperFile, cppClassName, matlabUniqueName,
      args);

  expanded += ("(" + args.names() + ")");
  if (returnVal.type1.name() != "void")
    returnVal.wrap_result(expanded, wrapperFile, typeAttributes);
  else
    wrapperFile.oss << "  " + expanded + ";\n";

  // finish
  wrapperFile.oss << "}\n";

  return wrapFunctionName;
}

/* ************************************************************************* */
void MethodBase::python_wrapper(FileWriter& wrapperFile, Str className) const {
  if(nrOverloads() == 1)
    // Example: .def("RzRxRy", &Rot3::RzRxRy)
    wrapperFile.oss << "  .def(\"" << name_ << "\", &" << className << "::"<< name_ << ")\n";
  else {
    // AUTOMATIC WRAPPING WITH OVERLOAD MACRO
    //     NOTE: DOES NOT WORK! See comments on Class::python_memberFunctionOverloads
    // wrapperFile.oss << ", " << className << "_" << name_ << "_overloads())\n";

    // MANUAL WRAPPING
    //     NOTE: Prototypes defined by calling Class::python_memberFunctionOverloads
    // Examples: .def("RzRxRy", Rot3_RzRxRy_0)
    //           .def("RzRxRy", Rot3_RzRxRy_1)
    SignatureGroupList sigGroupList = groupSignatureOverloads();
    for(size_t i=0; i < sigGroupList.size(); i++){
      wrapperFile.oss << "  .def(\"" << name_ << "\", " << python_funcPointerName(name_, i);
      if(sigGroupList[i].signatureList().size() > 1) {
        wrapperFile.oss << ", " << python_overloadName(className,name_,i) << "()";
      }
      if(sigGroupList[i].mainSignature().retValue.type1.isRef){
        wrapperFile.oss << ", return_internal_reference<>()";
      }
      wrapperFile.oss << ")\n";
    }

    // for(size_t i=0; i < nrOverloads(); i++){
    //   wrapperFile.oss << "  .def(\"" << name_ << "\", " << (isStatic()? "static_" : "") << className << "_" << name_ << "_" << i << ")\n";
    // }
  }
}

/* ************************************************************************* */
