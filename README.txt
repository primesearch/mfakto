** Preface for mfakto 0.16.0-beta.5 **

This is a developmental version of mfakto. It has been verified to produce
correct results. However, there may be bugs and incomplete features, and
performance may be suboptimal on some devices. Please help improve mfakto by
doing tests, providing feedback and reporting issues. Of course, code
contributions are always welcome.

You can get support via the following means:

- the official thread at the GIMPS forum:
  https://mersenneforum.org/showthread.php?t=15646
- opening a ticket on GitHub: https://github.com/primesearch/mfakto/issues

#################
# mfakto README #
#################

Table of contents

0      What is mfakto?
1      Compilation
1.1    Linux
1.2.1  Windows: MSVC
1.2.2  Windows: MinGW
1.3    macOS
2      Running mfakto
2.1    Supported GPUs
2.2    Linux and macOS
2.3    Windows
3      Getting work and reporting results
4      Known issues
4.1    Non-issues
5      Tuning
6      FAQ
7      Plans


##################
# 0 About mfakto #
##################

mfakto is an OpenCL port of mfaktc that aims to have the same features and
functions. mfaktc is a program that trial factors Mersenne numbers. It stands
for "Mersenne faktorisation* with CUDA" and was written for Nvidia GPUs. Both
programs are used primarily in the Great Internet Mersenne Prime Search. mfakto
can also run on CPUs, although this is not done in practice.

Primality tests are computationally intensive, but we can save time by finding
small factors. GPUs are very efficient at this task due to their parallel
nature. Only one factor is needed to prove a number composite.

mfakto uses a modified Sieve of Eratosthenes to generate a list of possible
factors for a given Mersenne number. It then uses modular exponentiation to
test these factors. You can find more details at the GIMPS website:
https://mersenne.org/various/math.php#trial_factoring

* portmanteau of the English word "factorisation" and the German word
"Faktorisierung"

#################
# 1 Compilation #
#################

General requirements:
- C and C++ development tools
- an OpenCL SDK

#############
# 1.1 Linux #
#############

Requires:
- C compiler
  - can be installed with "sudo apt install gcc"
- C++ compiler
  - can be installed with "sudo apt install g++"
- OpenCL headers
  - can be installed with "sudo apt install ocl-icd-opencl-dev"

Steps:
- cd src
- optional: run "make clean" to remove any build artifacts
- make
  - mfakto should compile without errors

#######################
# 1.2.1 Windows: MSVC #
#######################

Requires:
- Visual Studio 2022

Steps:
- launch Visual Studio and open the solution file mfaktoVS12.sln
- select Build > Build Solution to compile mfakto. IntelliSense will report
  errors in the code due to undefined identifiers, but they are safe to ignore
  as Visual Studio will automatically download and install the dependencies.
  You can then select Project > Rescan Solution to resolve the errors

########################
# 1.2.2 Windows: MinGW #
########################

Requires:
- MinGW (64-bit)
- an OpenCL SDK
- optional: MSYS2

Initial steps:
- download and install a 64-bit MinGW compiler. Our recommendation is to use
  MinGW-w64 as it is actively maintained: http://mingw-w64.org
- download and install the OpenCL SDK from AMD:
  https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases
- add the "bin" folder in the MinGW directory to your system Path variable
- verify that the AMD_APP_DIR variable in the makefile points to the SDK
  directory (see note)

MinGW can be optionally used with MSYS2 to compile mfakto:
- install MSYS2 using the instructions at the home page: https://www.msys2.org
- launch the MSYS2 shell and install the required packages:

      pacman -S mingw-w64-x86_64-gcc make

- start the 32-bit or 64-bit MinGW shell and navigate to the mfakto folder
- cd src
- make (cross your fingers)

Otherwise:
- navigate to the mfakto folder
- cd src
- mingw32-make

Additional notes:
- You may see some warnings, but they are safe to ignore.
- make does not support spaces in file names. If the path to the OpenCL SDK
  contains spaces, then you will need to either create a symbolic link or copy
  the files to another folder.
- mfakto may not compile with Win-builds. It is recommended to use the native
  Windows package instead: https://sourceforge.net/projects/mingw-w64/files
- To compile mfakto for both 32 and 64 bits, you will need to install MinGW-w64
  for both the i686 and x86_64 architectures.
- mfakto may give an "entry point not found" error on startup. Running make
  with the "static=yes" flag should prevent this.

#############
# 1.3 macOS #
#############

Requires:
- Command Line Tools

Steps:
- cd src
- optional: run "make clean" to remove any build artifacts
- make
  - mfakto should compile out of the box as macOS contains a native OpenCL
    implementation

####################
# 2 Running mfakto #
####################

General requirements:
- the latest drivers for the target device
  - AMD drivers:
    https://amd.com/en/support/download/drivers.html
  - OpenCL runtime for Intel CPUs:
    https://intel.com/content/www/us/en/developer/articles/technical/intel-cpu-runtime-for-opencl-applications-with-sycl-support.html

macOS users do not need any additional software as an OpenCL implementation is
included with the system.

Open a terminal window and run 'mfakto -h' for possible parameters. You may
also want to check mfakto.ini for additional settings. mfakto typically fetches
work from worktodo.txt as specified in the INI file. See section 3 on how to
obtain assignments and report results.

A typical worktodo.txt file looks like this:
  -- begin example --
  Factor=[assignment ID],66362159,64,68
  Factor=[assignment ID],3321932899,76,77
  -- end example --

You can launch mfakto after getting assignments. In this case, mfakto should
trial factor M66362159 from 64 to 68 bits, followed by M3321932899 from 76 to
77 bits.

mfakto has a built-in self-test that automatically optimizes parameters. Please
run 'mfakto -st' each time you:
- Recompile the code
- Download a new binary from somewhere
- Change the graphics driver
- Change your hardware

######################
# 2.1 Supported GPUs #
######################

AMD:
- all devices that support OpenCL 1.1 or later
- all APUs
- OpenCL 1.0 devices, such as the FireStream 9250 / 9270 and Radeon HD 4000
  series, can run mfakto but do not support atomic operations*
- not supported: FireStream 9170 and Radeon HD 2000 / 3000 series (as kernel
  compilation fails)

Other devices:
- Intel HD Graphics 4000 and later
- OpenCL-enabled CPUs via the '-d c' option
- Nvidia devices

* without atomics, mfakto may not correctly detect multiple factors found in
the same class. It may report only one factor or even an incorrect one (due to
mixed data from multiple factors). PrimeNet checks each factor and rejects
those that do not divide a Mersenne number. If this happens, run the exponent
and bit level again on a different device, or on the CPU using Prime95.
Lowering GridSize in mfakto.ini can reduce the chance of error.

#######################
# 2.2 Linux and macOS #
#######################

- build mfakto using the above instructions (only needs to be done once)
- go to the mfakto root folder and run "./mfakto" to launch the executable
- mfakto should run without any additional software

###############
# 2.3 Windows #
###############

OS-specific requirements:
- Microsoft Visual C++ Redistributable:
  https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist

Steps:
- build mfakto using the above instructions or download a stable version. Only
  the 64-bit binary is currently distributed.
- go to the mfakto root folder and launch the executable
- mfakto defaults to the first OpenCL-supported GPU it finds. Use the -d option
  to run mfakto on a specific device.

########################################
# 3 Getting work and reporting results #
########################################

You must have a PrimeNet account to participate. Simply go to the GIMPS website
at https://mersenne.org and click "Register" to create one. Once you've signed
up, you can get assignments in several ways.

Using the AutoPrimeNet application:
    AutoPrimeNet allows clients that do not natively support PrimeNet to obtain
    work and submit results. It is recommended to use this tool when possible.
    See the AutoPrimeNet download page for instructions:
    https://download.mersenne.ca/AutoPrimeNet

From the GIMPS website:
    Step 1) log in to the GIMPS website with your username and password
    Step 2) on the menu bar, select Manual Testing > Assignments
    Step 3) open the link to the manual GPU assignment request form
    Step 4) enter the number of assignments or GHz-days you want
    Step 5) click "Get Assignments"

    Users with older GPUs may want to use the regular form.

Using the GPU to 72 website:
    GPU to 72 "subcontracts" assignments from the PrimeNet server, and was
    previously the only means to obtain work at high bit levels. GIMPS now has a
    manual GPU assignment form that serves this purpose, but GPU to 72 remains
    a popular option.

    Please note results should be submitted to PrimeNet and not the GPU to 72
    website.

    GPU to 72 can be accessed here: https://gpu72.com

Using the MISFIT application:
    MISFIT is a Windows tool that automatically requests assignments and
    submits results. You can get it here: https://mersenneforum.org/misfit

    Important note: this program has reached end-of-life and is no longer
    supported. It is highly recommended to use AutoPrimeNet instead.

From mersenne.ca:
    James Heinrich's website mersenne.ca offers assignments for exponents up
    to 32 bits. You can get such work here: https://mersenne.ca/tf1G

    Be aware mfakto currently does not support exponents below 100,000.

A note on extending assignments:
    Because modern GPUs are much more efficient than CPUs, they are often used
    to search for factors beyond traditional Prime95 limits:
    https://mersenne.org/various/math.php

    Users have historically edited worktodo.txt to manually extend assignments,
    but this is no longer necessary as both the manual GPU assignment form and
    GPU to 72 allow higher bit levels to be requested. However, the PrimeNet
    server still accepts results whose bit levels are higher than assigned.

    Please do not manually extend assignments from GPU to 72 as users are
    requested not to "trial factor past the level you've pledged."

---

    Once you have your assignments, create an empty file called worktodo.txt
    and copy all the "Factor=..." lines into that file. Start mfakto, sit back
    and let it do its job. Running mfakto is also a great way to stress test
    your GPU. ;-)

---

Submitting results:
    It is important to submit the results once you're done. Do not report
    partial results as PrimeNet may reassign the exponent to someone else in
    the meantime; this can lead to duplicate work and wasted cycles.

    AutoPrimeNet automatically submits results in addition to obtaining
    assignments. For computers without Internet access, you can manually submit
    the results instead:

    Step 1) log in to the GIMPS website with your username and password
    Step 2) on the menu bar, select Manual Testing > Results
    Step 3) upload the results.json.txt file produced by mfakto. You may
            archive or delete the file after it has been processed.

    To prevent abuse, admin approval is required for manual submissions. You
    can request approval by contacting George Woltman at woltman@alum.mit.edu
    or posting on the GIMPS forum:
    https://mersenneforum.org/forumdisplay.php?f=38

    Important note: the results.txt file is deprecated and will no longer be
    accepted from 2025 onwards.

##################
# 4 Known issues #
##################

- On some devices, such as the Radeon HD 7700 - 7900 series, mfakto may be very
  slow at full GPU load due to fewer registers being available to the kernels.
  It will warn about this during startup.
  Set VectorSize=2 in mfakto.ini and restart mfakto to resolve this.

- The user interface has not been extensively tested against invalid inputs.
  Although there are some checks, they are not foolproof by any means.

- Your GUI may lag while running mfakto. On some Windows systems, the OS may
  restart the driver or even throw a BSoD in severe cases.
  Try lowering GridSize or NumStreams in your mfakto.ini file. Smaller grids
  should have better responsiveness at a slight performance loss. Another
  option for Windows users is to increase the GPU processing time:
  https://support.microsoft.com/en-us/help/2665946

- Could not find a supported GPU, falling back to CPU
  
  Possible reasons:
  - on Linux systems: there is no X server running
  - on Windows systems: the GPU is not the primary display adapter. One
    solution is to run mfakto on the main display rather than remotely
  - your graphics driver may be too old
  - the first device found is not an AMD GPU. Use the -d switch to specify a
    different device number. You can run 'clinfo' to get a list of devices
  - your system does not have a GPU installed

- on devices that do not support atomic operations, mfakto may give incorrect
  results when multiple factors are found in the same class. See the above
  "Supported GPUs" section for details.

- the '-d c' option fails for some CPUs; this is under investigation

- certain 15-bit Barrett kernels are incompatible with RDNA 2 and RDNA 3 GPUs,
  and need to be ported to 32 bits

- some have reported mfakto does not work on certain Nvidia hardware; this is
  also being investigated

##################
# 4.1 Non-issues #
##################

- mfakto runs slower on small ranges. Usually it doesn't make much sense to
  run mfakto with an upper limit below 64 bits. mfakto is designed to find
  factors between 64 and 92 bits, and is best suited for long-running jobs.

- mfakto can find factors outside the given range.
  This is because mfakto works on huge factor blocks, controlled by GridSize in
  the INI file. The default value of GridSize=3 means mfakto runs up to 1048576
  factor candidates at once, per class. So the last block of each class is
  filled up with factor candidates to above the upper bit level. This is a huge
  overhead for small ranges but can be safely ignored for larger ranges.
  For example, the average overhead is 0.5% for a class with 100 blocks but
  only 0.05% for one with 1000 blocks.

############
# 5 Tuning #
############

You can find additional settings in the mfakto.ini file. Read it carefully
before making changes. ;-)

#########
# 6 FAQ #
#########

Q: Does mfakto support multiple GPUs?
A: Currently no, but you can use the -d option to start an instance on a
   specific device. Please also see the next question.

Q: Can I run multiple instances of mfakto on the same computer?
A: Yes. In most cases, this is necessary to make full use of a GPU when sieving
   on the CPU. Otherwise, one instance should fully utilize a single GPU.

Q: What tasks should I assign to mfakto?
A: The 73-bit Barrett kernel is currently the fastest and works for factors
   between 60 to 73 bits. Selecting tasks for this kernel will give best
   results. However, the 79-bit Barrett kernel is quite fast too.

Q: I modified something in the kernel files, but my changes are not picked up
   by mfakto. How come?
A: mfakto tries to load the pre-compiled kernel files in version 0.14 and
   later. The INI file parameter UseBinfile defines the name of the file
   containing the pre-compiled kernels. You can force mfakto to recompile the
   kernels by deleting the file and restarting mfakto.

###########
# 7 Plans #
###########

- keep features/changes in sync with mfaktc
- performance improvements whenever I find them ;)
- documentation and comments in code
- full 95-bit implementation
- perftest modes for kernel speed.
- build a GCN-assembler-kernel
