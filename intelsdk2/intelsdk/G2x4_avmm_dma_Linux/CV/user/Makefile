######Makefile模板########
# 系统平台
ARCH = $(shell arch)
BITS =
ifeq "$(findstring x86_64,$(ARCH))" "x86_64"
BITS = 64
endif
# 目标文件名字
TARGET = k47_encode
# c源文件
SRC = $(wildcard src/*.c)\
 $(wildcard src/encode/*.c)

# object文件
OBJS = $(SRC:.c=.o)
OBJS_DIR = objs
OBJS_DEST = $(wildcard $(OBJS_DIR)/*.o)
# 编译器
CC = gcc
# 头文件路径，可以任意指定头文件
INCLUDE = \
 -I./src/include\
 -I./src/encode

# 链接库
LIBS := -lpthread
LOCAL_LIBS += ./lib$(BITS)
LIBS += `pkg-config --cflags --libs gtk+-3.0`
#LOCAL_LIBS += $(wildcard lib$(BITS)/libasf/*.a)
#LIBS += '-Wl,--start-group' $(LOCAL_LIBS) '-Wl,--end-group'
# 编译选项
#CFLAGS = -g -D __USE_LARGEFILE64
CFLAGS = -O2 -Wall
 
all:create $(TARGET)
# 定义目标文件生成规则
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS_DEST) $(LIBS)

.SUFFIXES:.c
# 定义obejct文件生成规则
.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -c $<
	mv *.o $(OBJS_DIR)
# 定义清理函数
.PHONY:clean
clean:
	rm -rf $(OBJS_DEST) $(TARGET)
create:
	mkdir objs -p
