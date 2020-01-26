RUNNING ON WINDOWS

>> There's a problem with MSCV. Since MSCV does some
   nonstandard stuff with their `Edit and Continue`
   when generating debug information. The compiler switch
   causing problems is `ZI`; change it to `Zi` to fix the
   issue. This is under Properties >> C/C++ >> All Options
   >> Debug Information Format.
