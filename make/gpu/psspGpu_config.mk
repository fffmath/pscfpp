#-----------------------------------------------------------------------
# This makefile fragment defines:
#
#   - A variable $(PSSP_DEFS) that is passed to the processor to define 
#     preprocessor flags that effect the code in the pssp/ directory. 
#
#   - A variable $(PSSP_MPI_SUFFIX) that is set to "_m" if MPI is enabled
#     and left undefined otherwise. This is added as a suffix to the name
#     of the pssp library.
#
#   - A variable $(PSSP_SUFFIX) that indicates what other features are
#     enabled, which is also added after PSSP_MPI_SUFFIX to the file
#     name of pssp library. 
#
#   - A variable $(PSSP_LIB) that the absolute path to the pssp library 
#     file.
#
# This file must be included by every makefile in the pssp directory. 
#-----------------------------------------------------------------------
# Flag to define preprocessor macros.

# Comments:
#
# The variable PSSP_DEFS is used to pass preprocessor definitions to
# the compiler, using the "-D" compiler option. If not empty, it must 
# consist of a list of zero or more preprocessor macro names, each 
# preceded by the compiler flag "-D".  
#
# The variable PSSP_SUFFIX is appended to the base name pssp.a of the 
# static library $(PSSP_LIB). 
#
 
# Initialize macros to empty strings
PSSP_DEFS=
PSSP_SUFFIX:=

# Each if-block below contains a line that appends a preprocessor macro 
# definition to PSSP_DEFS, thus defining an associated C++ preprocessor 
# macro. In some blocks, a second line add a suffix to the PSSP_SUFFIX 
# or PSSP_MPI_SUFFIX variable, thus changing the name of the pssp library
# and other libraries and executables that depend on the pssp library.

#-----------------------------------------------------------------------
# Macros related to use of external libraries

# CUDA libraries
PSSP_CUDA=1
ifdef PSSP_CUDA
  PSSP_GPU_DEFS+=-DPSSP_FFTW -DGPU_OUTER
#  PSSP_CUFFT_PREFIX=/usr/local/cuda
#  PSSP_CUFFT_INC=-I$(PSSP_CUFFT_PREFIX)/include
#  PSSP_CUFFT_LIB=-L$(PSSP_CUFFT_PREFIX)/lib -lcufft -lcudart
  PSSP_CUFFT_INC=
  PSSP_CUFFT_LIB=-lcufft -lcudart -lcuda -lcurand
  INCLUDES+=$(PSSP_CUFFT_INC)
endif

#-----------------------------------------------------------------------
# Path to the pssp library 
# Note: BLD_DIR is defined in config.mk

pssp_gpu_LIBNAME=pssp_gpu$(PSSP_SUFFIX)$(UTIL_SUFFIX)
pssp_gpu_LIB=$(BLD_DIR)/pssp_gpu/lib$(pssp_gpu_LIBNAME).a
#-----------------------------------------------------------------------
# Path to executable file

PSCF_PS_EXE=$(BIN_DIR)/pscf_pssp_gpu$(PSSP_SUFFIX)$(UTIL_SUFFIX)
#-----------------------------------------------------------------------