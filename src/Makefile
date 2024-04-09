# stuff

objout = casm
cc_flags = -I include

# compilers / assemblers
export cc = g++

# source files
c_src := $(shell find . -type f -name "*.cpp")

all: $(c_src)
	@echo "CC   $^"
	@$(cc) $(cc_flags) -o $(objout) $^
	@echo "$(objout) is ready"
