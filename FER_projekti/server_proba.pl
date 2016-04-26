use Dancer2;
use utf8;

set 'logger' 		=> 'file';
set 'log'		=> 'core';
set 'show_errors'  	=> 1;
set 'startup_info' 	=> 1;
set 'warnings'     	=> 1;

my $counter;
my $source_code = `cat server_proba.pl`;
$source_code =~ s#\n#<br />#g; 

hook before => sub { var time => scalar(localtime) };

get '/' => sub {
		headers 'Cache-control' => 'public';
		$counter++;
		my $tekst=`boinccmd --get_simple_gui_info`;
		$tekst =~ s#\n#<br />#g; 
		return "<title>Wow</title>Stranica je prikazana $counter puta!<br />$tekst<br /><img src='http://boincstats.com/signature/-1/user/3667514/sig.png'/>";
	};

get '/hello/:name' => sub {
    		my $name = route_parameters->get('name');
    		template 'hello.tt', { name => $name };
	};

get 'favicon.ico' => sub {
		return send_file( 'Network-Globe-Connected-icon.png');
	};

get '/source' => sub {
		return $source_code;
	};
	
dance;
