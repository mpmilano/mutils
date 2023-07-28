@PACKAGE_INIT@

set_and_check(mutils_INCLUDE_DIRS "@PACKAGE_CMAKE_INSTALL_INCLUDEDIR@")
set(mutils_LIBRARIES "-L@PACKAGE_CMAKE_INSTALL_LIBDIR@ -lmutils")
include("@PACKAGE_ConfigPackageLocation@/mutilsTargets.cmake")

check_required_components(mutils)
