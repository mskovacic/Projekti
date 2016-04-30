#! /usr/bin/perl

print "\nperl je bolji od pythona!\nSada je instaliran xterm pa neće biti problema!\nTako je\n";

while (<>) {
	tr/pvi/dug/;
	print;
	chomp;
	print `$_` if (m/^ls$/);
}


