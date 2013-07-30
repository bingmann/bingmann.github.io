#!/usr/bin/perl -w

use strict;
use warnings;

sub uniqlist(@) {
    my %h;
    map { $h{$_}++ == 0 ? $_ : () } @_;
}

sub file_maximum {
    my ($filename) = @_;
    my $maxvalue = 0;

    open(F, $filename) or die("Could not open $filename: $!");

    while( my $ln = <F> )
    {
	chomp($ln);
	my @val = split(/ /, $ln);
	($val[1]) or next;

	if ($maxvalue < $val[1]) {
	    $maxvalue = $val[1];
	}
    }

    close(F);

    return $maxvalue;
}

sub subdir_table {
    my ($subdir) = @_;

    # read all result files from subdirectory
    opendir(D, $subdir) or die("Could not readdir $subdir: $!");

    my %table;
    my %ciphers;

    foreach my $file (sort readdir(D))
    {
	(! -d $file) or next;

	my $maxvalue = file_maximum("$subdir/$file");

	my @fn = split(/-/, $file);

	$table{$fn[0]}{$fn[1]} = $maxvalue;
	$ciphers{$fn[1]} = 1;
    }

    closedir(D);

    # output maximum values as a tabbed table

    my @liblist = (uniqlist(qw(gcrypt mcrypt botan cryptopp openssl nettle beecrypt tomcrypt), sort keys %table));
    my @ciplist = (uniqlist(qw(), sort keys %ciphers));

    print "$subdir";
    foreach my $lib (@liblist)
    {
	print "\t$lib";
    }
    print "\taverage";
    print "\n";

    foreach my $cipher (@ciplist)
    {
	my $sum = 0;
	my $num = 0;

	print "$cipher";
	foreach my $lib (@liblist)
	{
	    print "\t";
	    if ($table{$lib}{$cipher}) {
		print $table{$lib}{$cipher} / 1024.0;
		$sum += $table{$lib}{$cipher};
		$num++;
	    }
	    else {
		print "";
	    }
	}
	print "\t".($num == 0 ? "" : ($sum / $num / 1024.0));
	print "\n";
    }
    print "\n";
}

foreach my $arg (@ARGV)
{
    subdir_table($arg);
}
