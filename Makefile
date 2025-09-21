default:
	mkdir -p build && clang -g -Og -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wshadow -Wwrite-strings \
	-Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -fsanitize=address \
	-fsanitize=undefined -fno-sanitize-recover=all -fstack-protector-strong -D_FORTIFY_SOURCE=2 -std=c18 main.c -o build/main \
	-lraylib -lm -lpthread -ldl -lrt -lX11 

release:
	mkdir -p build && gcc -O3 -DNDEBUG -march=native -mtune=native -flto -ffast-math -funroll-loops -fprefetch-loop-arrays \
	-fomit-frame-pointer -pipe -std=c18 main.c -o build/main \
	-lraylib -lm -lpthread -ldl -lrt -lX11 

clean:
	rm -rf build/*