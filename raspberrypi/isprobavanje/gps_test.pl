use GPS::NMEA;
my $gps = GPS::NMEA->new(
    Port => '/dev/ttyAMA0',
    Baud => 9600,
);

while(1) {
    my($ns,$lat,$ew,$lon) = $gps->get_position;
    my $alt = 0; #$gps->get_altitude;
    # decimal portion is arcminutes, so convert to degrees
    $lat = int($lat) + ($lat - int($lat)) * 1.66666667;
    $lon = int($lon) + ($lon - int($lon)) * 1.66666667;

    print "$ns,$lat,$ew,$lon,$alt\n";
}

