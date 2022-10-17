#include "yaml-cpp/exceptions.h"

// This is here for compatibility with older versions of Visual Studio
// which don't support noexcept
#ifdef _MSC_VER
    #define YAML_CPP_NOEXCEPT _NOEXCEPT
#else
    #define YAML_CPP_NOEXCEPT noexcept
#endif

namespace YAML {

// These destructors are defined out-of-line so the vtable is only emitted once.
Exception::~Exception()  {}
ParserException::~ParserException()  {}
RepresentationException::~RepresentationException()  {}
InvalidScalar::~InvalidScalar()  {}
KeyNotFound::~KeyNotFound()  {}
InvalidNode::~InvalidNode()  {}
BadConversion::~BadConversion()  {}
BadDereference::~BadDereference()  {}
BadSubscript::~BadSubscript()  {}
BadPushback::~BadPushback()  {}
BadInsert::~BadInsert()  {}
EmitterException::~EmitterException()  {}
BadFile::~BadFile()  {}
}

#undef YAML_CPP_NOEXCEPT


