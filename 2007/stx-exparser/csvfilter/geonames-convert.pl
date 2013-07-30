#!/usr/bin/perl -w

use strict;
use warnings;

=head1 Convery All geonames.org originals

for f in geonames-original/*.txt.bz2; do
   bzcat $f | perl geonames-convert.pl > geonames-data/`basename ${f%%.txt.bz2}`.csv;
done

=cut

# Load admin1 and admin2 lookup tables

my %admin1;
open(ADM, "geonames-admin1Codes.txt") or die;

while( my $ln = <ADM> )
{
    chomp($ln);
    next if !$ln;

    my @row = split(/\t/, $ln);
    $admin1{$row[0]} = $row[1];
}

close(ADM);

my %admin2;
open(ADM, "geonames-admin2Codes.txt") or die;

while( my $ln = <ADM> )
{
    chomp($ln);
    next if !$ln;

    my @row = split(/\t/, $ln);
    $admin2{$row[0]} = $row[1];
}

close(ADM);

# Process CSV files

my @data;
my @header = qw(geonameid name asciiname alternatenames latitude longitude feature_class feature_code country cc2 admin1 admin2 population elevation gtopo30 timezone lastupdate);
my %headermap = map({ $header[$_] => $_ } 0..@header-1);

my @splicecols = qw(lastupdate cc2 feature_code feature_class alternatenames asciiname);

splice(@header, $headermap{$_}, 1) foreach (@splicecols);

print join("\t", @header)."\n";

while( my $ln = <> )
{
    chomp($ln);
    next if !$ln;

    my @row = split(/\t/, $ln);

    next if $row[$headermap{feature_class}] ne "P";
    next if $row[$headermap{feature_code}] !~ /^PPL[ACGLRSX]?$/;

    my $admin1lookup = @row[$headermap{country}] . "." . @row[$headermap{admin1}];
    my $admin2lookup = @row[$headermap{country}] . "." . @row[$headermap{admin1}] . "." . @row[$headermap{admin2}];

    if (defined $admin1{$admin1lookup}) {
	@row[$headermap{admin1}] = $admin1{ $admin1lookup };
    }
    if (defined $admin2{$admin2lookup}) {
	@row[$headermap{admin2}] = $admin2{ $admin2lookup };
    }

    splice(@row, $headermap{$_}, 1) foreach (@splicecols);

    print(join("\t", @row)."\n");
}
