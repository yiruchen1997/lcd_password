all:a.out clean

a.out: lex.yy.c y.tab.c
	   gcc lex.yy.c y.tab.c -ly -lfl
lex.yy.c: B054020033.l
	       flex B054020033.l
y.tab.c: B054020033.y
	      bison -y -d B054020033.y

clean:
	      rm lex.yy.c y.tab.c y.tab.h
