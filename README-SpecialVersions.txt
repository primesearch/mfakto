Different mfakto variants can be compiled for special purposes. Here are the
three most common configurations:

mfakto-64k :  Optimized for CPUs with 64 kB of L1 cache

              Useful for AMD CPUs, but also Intel CPUs when the SievePrimes
              value is very high (around 100,000 or above).

              Set the SIEVE_SIZE_LIMIT define to 64 in params.h to compile
              this version.

mfakto-var :  Enables configuration of SieveSize via mfakto.ini

              Useful for determining the optimal SieveSize value, but is about
              1-3% slower compared to versions in which the sieve size is
              fixed at compile time.

              Comment out the SIEVE_SIZE_LIMIT define in params.h to create
              this build.

mfakto-pi  :  Displays the OpenCL performance information for each block

              Enables accurate measurement of certain metrics, such as the
              kernel execution speed and data transfer rate. Intended for
              performance tests.

              Uncomment the CL_PERFORMANCE_INFO define in params.h to
              compile.

Otherwise, these versions are the same as the normal mfakto executable and are
optimized for CPUs with 32 kB of L1 cache.

