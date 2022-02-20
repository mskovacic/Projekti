use strict;
use Dancer2;

set 'session'      => 'Simple';
set 'template'     => 'template_toolkit';
set 'logger'       => 'console';
set 'log'          => 'debug';
set 'show_errors'  => 1;
set 'startup_info' => 1;
set 'warnings'     => 1;

my $template = '
<h1>ISPIS DOKUMENATA</h1>
<br>
<form method="post" enctype="multipart/form-data" action="/">
<input type="file" name="fileupload" value="fileupload" id="fileupload">
<label for="fileupload">Odaberi datoteku za ispis</label>
<input type="submit">
</form>
';

get '/' => sub {
	return $template;
};

post '/' => sub {
	#my $fileupload = body_parameters->get('fileupload');
	#print $fileupload;
	#print "------\n";
	my $x = request->upload('fileupload');
	#print $x->content;
	#print "\n";
	#`echo "$x->content" | lp`;
	print $x->copy_to($x->tempname)."\n";
	`lp ${\$x->tempname}`;
	redirect '/';
};

start;
