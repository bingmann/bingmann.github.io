#!/usr/bin/perl -w

use strict;
use warnings;

use lib("../../");
use idlebox::CSV2HTML;

my $csv2html = idlebox::CSV2HTML->new();
$csv2html->class_column(1);

$csv2html->process_fh(\*STDIN);
$csv2html->print();
