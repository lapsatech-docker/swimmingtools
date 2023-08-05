 PHP_ARG_ENABLE(swt,
  [Whether to enable the "swt" extension],
  [  —enable-swt        Enable "swt" extension support])

if test $PHP_SWT != "no"; then
  PHP_REQUIRE_CXX()
  PHP_SUBST(SWT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, SWT_SHARED_LIBADD)
  PHP_NEW_EXTENSION(swt, *.cpp, $ext_shared,, "-std=c++0x", "yes")
fi
