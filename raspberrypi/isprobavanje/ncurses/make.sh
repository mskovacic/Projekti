for i in {1..28};
do
    gcc -lmenu -lform -lncurses -lpanel ncurses_proba$i.c -o proba$i.o; 
done
