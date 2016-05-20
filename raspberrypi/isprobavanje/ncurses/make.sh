for i in *.c;
do
    #echo "compiling $i";
    output_name=`echo $i | sed -r "s/[a-zA-Z]*([0-9]+)\.c/proba\1.o/"`;
    gcc -lmenu -lform -lncurses -lpanel $i -o $output_name;
done
