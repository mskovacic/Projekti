use Device::GPS;
use Device::GPS::Connection::Serial;

my $gps = Device::GPS->new({
    connection => Device::GPS::Connection::Serial->new({
        port => '/dev/ttyAMA0',
        baud => '9600',
    }),
});
$gps->add_callback( $gps->CALLBACK_POSITION, sub {
    my ($time, $lat_deg, $lat_min, $lat_sec, $ns,
        $long_deg, $long_min, $long_sec, $ew,
        $quality, $satellites, $horz_dil, $altitude, $height, 
        $time_since_last_dgps, $dgps_station_id) = @_;
    print "Lat: $lat_deg deg $lat_min.$lat_sec' $ns\n";
    print "Long: $long_deg deg $long_min.$lat_sec' $ew\n";
});
$gps->add_callback( $gps->CALLBACK_VELOCITY, sub {
    print "@_\n";
}); 
 
while(1) { $gps->parse_next }

