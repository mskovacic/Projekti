for i in *.c;
do
    #echo "compiling $i";
    output_name=`echo $i | sed -r "s/\.c/.o/"`;
    gcc -lmenu -lform -lncurses -lpanel $i -o $output_name;
done
