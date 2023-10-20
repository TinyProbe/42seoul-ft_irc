##########################################################
# Reference : https://greenfishblog.tistory.com/150
##########################################################
#
# make all
# make [debug | release]
# make clean [debug | release]
# make fclean [debug | release]
# make re
#
##########################################################

##################################
# 빌드의 기본값은 debug이다.
TARGET    = ircserv
CC        = c++
CXXFLAGS  = -std=c++98 -Wall -Wextra -Werror -I./include
LFLAGS    =
CONFIG    = debug
BUILD_DIR = build/$(CONFIG)
NAME      = $(BUILD_DIR)/$(TARGET)

##################################
# makefile에 들어온 argument를 구한다.
ifneq "$(findstring clean, $(MAKECMDGOALS))" ""
  ARG.CLEAN = 1
else ifneq "$(findstring all, $(MAKECMDGOALS))" ""
  ARG.DEBUG = 1
endif

ifneq "$(findstring release, $(MAKECMDGOALS))" ""
  ARG.RELEASE = 1
else ifneq "$(findstring debug, $(MAKECMDGOALS))" ""
  ARG.DEBUG = 1
else ifneq "$(findstring re, $(MAKECMDGOALS))" ""
  ARG.RE = 1
endif

##################################
# DEBUG / RELEASE 빌드를 설정한다.
ifeq ($(ARG.RELEASE),1)
  # release에 특화된 설정을 한다.
  CXXFLAGS += -O2 -D_RELEASE_
  CONFIG    = release
else ifeq ($(ARG.DEBUG),1)
  # debug에 특화된 설정을 한다.
  CXXFLAGS += -g3 -D_DEBUG_
  CONFIG    = debug
endif

##################################
# makefile 실행 처리
.PHONY: debug release build all clean fclean re PRE_BUILD POST_BUILD

# 일반적인 Build의 스텝을 결정한다.
BUILD_STEP = $(NAME) POST_BUILD

# Makefile의 최종 Target과 Depend를 결정한다.
ifeq ($(ARG.RE),1)
  # re인 경우...
  re: | fclean $(BUILD_STEP)
  debug: ; @true
  release: ; @true
else ifeq ($(ARG.CLEAN),1)
  # clean인 경우...
  debug: ; @true
  release: ; @true
else
  # clean/re와 함께 쓰이지 않은 경우...
  build: | $(BUILD_STEP)
  all: build
  debug: build
  release: build
endif

#--------------------------------
# 주의 : ../../ 와 같이 상위 directory로 넘어가는 source는 가져오면 안된다.
# 왜냐하면, ./Build/ 밑으로 모든 빌드 파일이 들어가야 하는데,
# ../../등이 포함되면 ./Build/ 상위로 빌드 파일이 들어갈 수 있기 때문에
# 위험하다. 즉, makefile의 위치는 root directory에 있어야 한다.

##################################
# include/...
INC_LST = common.h \
					type.h \
					Program.h \

# source/...
SRC_LST = main.cpp \
					Program.cpp \

INC = $(addprefix include/,$(INC_LST))
OBJ = $(addprefix $(BUILD_DIR)/,$(SRC_LST:.cpp=.o))

##################################
# [Link] Link Part
# 빌드된 .o 들을 링크하여 NAME을 생성한다. 
$(NAME): Makefile $(INC) $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(NAME)

##################################
# [Compile] Compile Part
$(OBJ): $(BUILD_DIR)/%.o: source/%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c -o $@ $<
	
##################################
# Build가 시작되었다.
# 필요한 전처리가 있다면 구현한다.
PRE_BUILD:
	@echo 
	@echo ========================================
	@echo Make file started [config =\> $(CONFIG)]
	@echo ========================================

##################################
# Build가 종료되었다.
# 필요한 후처리가 있다면 구현한다.
POST_BUILD:
	@echo 
	@echo ========================================
	@echo Make file finished [config =\> $(CONFIG)]
	@echo ========================================

##################################
# Clean up 한다.
clean:
	@echo 
	rm -f $(OBJ)
	@echo ========================================
	@echo Clean work finished [config =\> $(CONFIG)]
	@echo ========================================

##################################
# Full Clean up 한다.
fclean: clean
	@echo 
	rm -f $(NAME)
	@echo ========================================
	@echo FClean work finished [config =\> $(CONFIG)]
	@echo ========================================
