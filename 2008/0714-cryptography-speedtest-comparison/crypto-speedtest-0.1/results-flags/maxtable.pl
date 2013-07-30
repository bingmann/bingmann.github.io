#!/usr/bin/perl -w

use strict;
use warnings;

my %table;
my %tablesum;

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

    foreach my $file (sort readdir(D))
    {
	(-d $file) and next;

	my $filekey = $file;
	$filekey =~ s/-ecb\.txt$//;

	$table{$subdir}{$filekey} = file_maximum("$subdir/$file");

	$tablesum{$subdir} += $table{$subdir}{$filekey};
    }

    closedir(D);
}

foreach my $arg (@ARGV)
{
    subdir_table($arg);
}

# output maximum values as a tabbed table

# output header

my @filelist = uniqlist sort map(keys %{$table{$_}}, keys %table);

foreach my $file (@filelist)
{
    print "\t$file";
}
print "\taverage";
print "\tsum";
print "\n";

# sort body



# output body

foreach my $subdir (sort { $tablesum{$b} <=> $tablesum{$a} } keys %table)
{
    print "$subdir";
    my $sum = 0;
    my $count = 0;
    foreach my $file (@filelist)
    {
	print "\t".($table{$subdir}{$file} / 1024.0);
	$sum += $table{$subdir}{$file};
	$count++;
    }
    print "\t".($sum / $count / 1024.0);
    print "\t".($sum / 1024.0);
    print "\n";
}
