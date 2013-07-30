#!/usr/bin/perl -w

use strict;
use warnings;

open(DS, "> datasets.csv") or die();

open(DH, "datasets.head") or die();
print(DS join("", <DH>));
close(DH);

# load geonames country list

if (1) {
    open(CI, "geonames-countryInfo.txt") or die;

    my @header;
    my %header;

    my %keys;
    my %datafiles;
    my %names;
    my %comment;
    my %lines;

    while ( my $ln = <CI> )
    {
	chomp($ln);
	next if !$ln;
	next if $ln =~ /^#/;

	my @row = split(/\t/, $ln);

	if (!@header)
	{
	    @header = @row;
	    %header = map({ $header[$_] => $_ } 0..@header-1);
	}
	else
	{
	    my $isocode = $row[$header{"iso alpha2"}];
	    my $name = $row[$header{"name"}];

	    my $lines = 0;
	    if (open(GD, "geonames-data/$isocode.csv"))
	    {
		my @lines = <GD>;
		close(GD);
		$lines = scalar(@lines) - 1;
	    }

	    $lines > 0 or next;

	    $keys{$name} = "geonames-$isocode";
	    $datafiles{"geonames-$isocode"} = "geonames-data/$isocode.csv";
	    $names{"geonames-$isocode"} = "Cities in $name";
	    $comment{"geonames-$isocode"} = 'Also imported from <a class="exp" href="http://www.geonames.org/">geonames.org</a> - '."Country code $isocode - $name";
	    $lines{"geonames-$isocode"} = $lines;
	}
    }

    close(CI);

    foreach my $k (sort keys %keys)
    {
	my $v = $keys{$k};
	print(DS "$v | $datafiles{$v} | $names{$v} | $comment{$v} | $lines{$v}\n");
    }
}

close(DS);
