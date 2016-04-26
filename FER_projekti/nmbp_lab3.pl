#!/usr/bin/perl
#https://metacpan.org/pod/distribution/MongoDB/lib/MongoDB/Examples.pod
#http://base64.wutils.com/encoding-online/image-to-base64/
#http://search.cpan.org/~friedo/MongoDB-0.502.0/lib/MongoDB/BSON/Binary.pm
#http://perldoc.perl.org/MIME/Base64.html
#http://localhost:3000/vijesti
#http://localhost:3000/unos

    use Dancer;
	use MongoDB;
	use MIME::Base64;
	use WWW::Mechanize;
	use utf8;
	use Data::Dumper;

	my $client=MongoDB->connect();
	my $db = $client->get_database( 'nmbp' );
	my $vijesti = $db->get_collection( 'vijesti' );
	
	my @cols=$db->collection_names;
	my $create_capped = [ create => "najnovije", capped => true, size => 500, max => 10 ];
	$db->run_command($create_capped) unless ('najnovije' ~~ @cols);
	
	my $najnovije = $db->get_collection( 'najnovije' );
	
    get '/vijesti' => sub {
		#my $sve_vijesti = $vijesti->find()->sort({"_id" => -1})->limit(10);
		#my $sve_vijesti = $db->get_collection('vijesti')->aggregate([{'$sort' => {"_id" => -1}}, {'$limit'=> 10 }]);
		my $najnovije_vijesti = $najnovije->find();
		my $response = '<table style="width:100%">';
		while (my $kljuc = $najnovije_vijesti->next) {
			my $doc = $vijesti->find({"_id" => MongoDB::OID->new($kljuc->{'id'})})->next;
			$response.='<tr><td><h2>'.$doc->{'title'}.'</h2><br />'.$doc->{'text'}.'<br /><br /><h4>'.$doc->{'author'}.'</h4></td>';
			$response.='<td>'.get_picture($doc->{'picture_bytes'}).'</td>';
			#local $"="\n";
			my $comments= join('<br />',map "&middot;$_",@{$doc->{'comments'}});
			$response.='<td>Komentari:<br />'.$comments.'<br /><form action="/vijesti?id='.$doc->{'_id'}.'" method="post" name="comment_input"><input type="text" name="comment" required><br /><input type="submit" value="Unesi"></form></td></tr>';
		}
		$response.='</table>';
        return $response;
    };
	
	post '/vijesti' => sub {
		my $comment = params->{'comment'};
		my $document_id = param 'id';
		chomp($comment);
		my $result = $vijesti->update_many({"_id" => MongoDB::OID->new($document_id)}, {'$push' => {'comments' => "$comment"}});
		#return $document_id.$comment;
		redirect '/vijesti';
	};
	
	get '/' => sub { 
		redirect "/vijesti";
	};
	
	get '/unos' => sub {
		my $id="";
		if (defined param 'id') {
			my $param_id=param 'id';
			$id="Uspjesno unesen zapis s id: $param_id!<br /><br />";
		}
		my $forma=	'<form action="unos" method="post" name="document_input">
						Naslov: <input type="text" name="title" required><br />
						Tekst: <textarea name="text" cols="40" rows="5" required></textarea><br />
						Autor: <input type="text" name="author" required><br />
						Url slike: <input type="url" name="picture_url" required><br />
						<input type="submit" value="Unesi">
					</form>';
		return $id.$forma;
	};
    
	post '/unos' => sub {
		my $pic = MongoDB::BSON::Binary->new(data => get_picture_bytes(params->{'picture_url'}));
		my $title = params->{'title'};
		my $text = params->{'text'};
		my $author = params->{'author'};
		chomp($title);
		chomp($text);
		chomp($author);
		my $result = $vijesti->insert_one({	
			"title" => $title,
			"text" => $text,
			"author" => $author,
			"picture_bytes" => $pic,
			"comments" => []
		});
		my $id = $result->inserted_id;
		$najnovije->insert_one({"id"=>$id});
		redirect "/unos?id=$id";
	};
	
	get '/map-reduce1' => sub {
		my $map1 = "function() {
						emit(this._id, this.comments.length);
					};";
	
	
		my $reduce1= "function(key, values) {
						return {id:key , komentari:values};
					};";
	
		my @lista;
		my %h;
		my $idx = Tie::IxHash->new(mapreduce => 'vijesti', 'map' => $map1, reduce => $reduce1, 'out' => 'coll1');
		my $result = $db->run_command($idx);
		my $res_coll = $result->{'result'};
		my $collection = $db->get_collection($res_coll);
		my $svi_dokumenti = $collection->find();
		while (my $dok = $svi_dokumenti->next) {
			$h{$dok->{'_id'}}=$dok->{'value'};
		}
		foreach my $val (sort {$h{$b} cmp $h{$a}} keys %h) {
			push @lista, "$val:$h{$val}";
		}
		return join('<br />',@lista);
	};
	
	get '/map-reduce2' => sub {
		my $map2= "function() {
					var splitted=this.text.split(); 
					for( var i=0; i<splitted.length; i++) {
						emit(this.author, splitted[i].toLowerCase());
					}
				};";
	
	
		my $reduce2= "function(key, values) {
					var author=key; 
					var ponavljanja={}; 
					for (var i=0; i<values.length; i++) {
						if (!(values[i] in ponavljanja)) {ponavljanja[values[i]]=0;}
						ponavljanja[values[i]]+=1;
					}
					var sortable = [];
					for (var i in ponavljanja) sortable.push([i, ponavljanja[i]]);
					sortable.sort(function(a, b) {return b[1] - a[1]});
					var list =[];
					for (var i=0; i<sortable.length; i++ ) { if (i>=10) break; list.push(sortable[i][0]);}
					return {_id:author, lista:list};
				};";
		my $idx = Tie::IxHash->new(mapreduce => 'vijesti', 'map' => $map2, reduce => $reduce2 , 'out' => 'coll2');
		my $result = $db->run_command($idx);
		my $res_coll = $result->{'result'};
		my $collection = $db->get_collection($res_coll);
		my $svi_dokumenti = $collection->find();
		my @lista;
		while (my $dok = $svi_dokumenti->next) {
			push @lista, $dok->{'_id'}.':'.$dok->{'value'};
		}
		return join('<br />',@lista);
	};
	
    dance;
	
	sub get_picture_bytes($) {
		my $mech = WWW::Mechanize->new();
		#my $url = "http://www.index.hr/images2/H20160111000678_milanovic.jpg";
		$mech->get( shift );
		my $content = $mech->content();
		#my $picture = encode_base64($content);
		#return "<img src=\"data:image/jpeg;base64,$picture\" alt=\"\">";
		return $content;
	};
	
	sub get_picture($) {
		my $picture = encode_base64(shift);
		return "<img src=\"data:image/jpeg;base64,$picture\" alt=\"\">";
	}
