include $(SRC_DIR)/fd1d/solvers/sources.mk
include $(SRC_DIR)/fd1d/iterator/sources.mk
include $(SRC_DIR)/fd1d/sweep/sources.mk

fd1d_=\
  $(fd1d_solvers_) \
  $(fd1d_iterator_) \
  $(fd1d_sweep_) \
  fd1d/Domain.cpp \
  fd1d/GeometryMode.cpp \
  fd1d/System.cpp 

fd1d_SRCS=\
     $(addprefix $(SRC_DIR)/, $(fd1d_))
fd1d_OBJS=\
     $(addprefix $(BLD_DIR)/, $(fd1d_:.cpp=.o))

$(fd1d_LIB): $(fd1d_OBJS)
	$(AR) rcs $(fd1d_LIB) $(fd1d_OBJS)

