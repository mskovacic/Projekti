#prvi redak
#drugi redak
echo "ovo je prva naredba"
echo "bzvz"
echo "ovo je pisano u nano!"
ls *| wc -w | sed -r 's/([0-9]+)/U ovom direktoriju ima \1 fajlova/g'
