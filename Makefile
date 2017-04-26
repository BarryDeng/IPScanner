#使用$(wildcard *.c)来获取工作目录下的所有.c文件的列表
sources:=$(wildcard *.c) $(wildcard scanner/src/*.c)
objects:=$(sources:.c=.o)
#这里,dependence是所有.d文件的列表.即把串sources串里的.c换成.d
dependence:=$(sources:.c=.d)

#所用的编译工具
CC=gcc
LDFLAGS=-lnet -lpcap
INCLUDE_DIR=scanner/include/
OBJ_DIR=obj/
PROG=ipscan

all: $(PROG)

#当$(objects)列表里所有文件都生成后，便可调用这里的 $(CC) $^ -o $@ 命令生成最终目标all了
#把all定义成第1个规则，使得可以把make all命令简写成make
$(PROG): $(objects)
	$(CC) $(addprefix $(OBJ_DIR),$^) -o $@ $(LDFLAGS)

#这段是make的模式规则，指示如何由.c文件生成.o，即对每个.c文件，调用gcc -c XX.c -o XX.o命令生成对应的.o文件。
#如果不写这段也可以，因为make的隐含规则可以起到同样的效果
%.o: %.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $(addprefix $(OBJ_DIR),$@)

include $(dependence) #注意该句要放在终极目标all的规则之后，否则.d文件里的规则会被误当作终极规则了
%.d: %.c
	set -e; rm -f $@; \
	$(CC) -MM -I$(INCLUDE_DIR) $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	

.PHONY: clean #之所以把clean定义成伪目标，是因为这个目标并不对应实际的文件
clean:
	rm -f $(PROG) $(addprefix $(OBJ_DIR),$(objects)) $(dependence)
