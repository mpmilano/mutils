cmake_minimum_required (VERSION 3.1)
project (mutils)

#CXX FLAGS
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++14 -fPIC -g")
set(CMAKE_SHARED_LINKER_FLAGS "-shared --enable-new-dtags")

include_directories(${mutils_SOURCE_DIR}/include)

add_library(mutils SHARED utils.cpp abiutils.cpp)

set_target_properties(mutils PROPERTIES PUBLIC_HEADER "include/mutils/17_type_utils.hpp;include/mutils/17utils.hpp;include/mutils/AtScopeEnd.hpp;include/mutils/CTString.hpp;include/mutils/CTString_decl.hpp;include/mutils/CTString_impl.hpp;include/mutils/CTString_macro.hpp;include/mutils/CTString_split.hpp;include/mutils/CTString_strip_paren_group.hpp;include/mutils/FunctionalMap.hpp;include/mutils/Hertz.hpp;include/mutils/PrintableObject.hpp;include/mutils/abiutils.hpp;include/mutils/args-finder.hpp;include/mutils/argswrapper.hpp;include/mutils/as_value.hpp;include/mutils/backtrace.hpp;include/mutils/cexprutils.hpp;include/mutils/compile-time-lambda.hpp;include/mutils/compile-time-tuple.hpp;include/mutils/cstring.hpp;include/mutils/extras;include/mutils/filter-varargs.hpp;include/mutils/macro_utils.hpp;include/mutils/mutils.hpp;include/mutils/peano.hpp;include/mutils/private_access.hpp;include/mutils/restrict.hpp;include/mutils/tuple_extras.hpp;include/mutils/type_utils.hpp;include/mutils/typelist.hpp;include/mutils/typeset.hpp;include/mutils/variant.hpp")
install(TARGETS mutils
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/mutils/)
