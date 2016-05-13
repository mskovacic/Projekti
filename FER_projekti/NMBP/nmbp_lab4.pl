#! /usr/bin/perl
#dodati Dancer2 za webserver,sparql upite slati pomoæu RDF::Query

use RDF::Helper;
use RDF::Trine::Store::DBI;
use utf8;
use TryCatch;
use RDF::Query::Client;
use RDF::Query;
use feature ":5.10";
use constant DBPEDIA => 'http://dbpedia.org/sparql';
use constant LMDB => 'http://data.linkedmdb.org/sparql';

my $db_handle = DBI->connect('DBI:Pg:dbname=perlrdf', 'postgres', 'postgres');
my $store = RDF::Trine::Store::DBI->new('model1', $db_handle );

my $rdf = RDF::Helper->new(
    BaseInterface => 'RDF::Trine',
	Model => RDF::Trine::Model->new($store),
    namespaces => {
        rdf => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#',
		owl => 'http://www.w3.org/2002/07/owl#',
		xsd => 'http://www.w3.org/2001/XMLSchema#',
		rdfs => 'http://www.w3.org/2000/01/rdf-schema#',
		foaf => 'http://xmlns.com/foaf/0.1/',
		oddlinker => 'http://data.linkedmdb.org/resource/oddlinker/',
		map1 => 'file:/C:/d2r-server-0.4/mapping.n3#',
		db => 'http://data.linkedmdb.org/resource/',
		dbpedia => 'http://dbpedia.org/property/',
		skos => 'http://www.w3.org/2004/02/skos/core#',
		dc => 'http://purl.org/dc/terms/',
		movie => 'http://data.linkedmdb.org/resource/movie/'
   },
   ExpandQNames => 1
);

print $rdf->count()." zapisa u bazi!\n";
open ($fh, '>>', "lista_neuspjelih.txt") or die "file nije napravljen!";

for ($i=844; $i<1050; $i++){
	$offset = 400 * $i;
	$rez = napuni_bazu($offset);
	$br_pokusaja=0;
	sleep(1);
	while (!defined $rez and $br_pokusaja<3) {
		sleep(5);
		$rez = napuni_bazu($offset);
		$br_pokusaja++;
	}
	unless (defined $rez){
		print "Neuspio offset $i\n";
		print $fh "$i\n";
	}
	else {
		print "Upisan offset $i\n";
		print $rdf->count() ." zapisa u bazi!\n";
	}
}

sub napuni_bazu {
	$offset = shift;
	my $query = RDF::Query::Client->new("SELECT  ?s ?p ?o WHERE {?s ?p ?o} OFFSET $offset LIMIT 400");
	#print $query->as_sparql;
	#my $iterator = $query->execute('http://www.linkedmdb.org/snorql/');
	my $iterator = $query->execute(LMDB);

	if (defined $iterator) {
		$brojac = 0;
		while (my $row = $iterator->next) {
			$brojac++;
			#print $brojac,$row->{'s'},$row->{'p'},$row->{'o'}."\n";
			$res_s = $rdf->new_resource($row->{'s'}->as_string);
			$res_p = $rdf->new_resource($row->{'p'}->as_string);
			$res_o = $rdf->new_resource($row->{'o'}->as_string);
			$rdf->assert_resource($res_s,$res_p,$res_o);
		   #foreach my $k (keys %$row)
		   #{
		   #         my $v = $row->{$k};
		   #         say sprintf("%s=%s", $k, $v->as_string);
		   # }
		}
		return $brojac;
	}
	else {
		#print "iterator undef\n";
		return undef;
	}
}


#foreach (keys %($query->http_response)) {
#	print $_, $($query->http_response{$_});
#}
