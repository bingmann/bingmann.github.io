#!/usr/bin/perl -w

use strict;
use warnings;

use Git;
use JSON::XS;

# load repositories from index.js
my %repos;
open(P, "index.js") or die;
while (my $ln = <P>) {
    next unless $ln =~ /^makeButton\([^,]+, "([^"]+)", "([^"]+)"\);/;
    $repos{$1} = $2;
}
close(F);

# some overrides
$repos{TBTrader} = 'git@vpan:2015/tbtrader.git';
$repos{Parsec} = 'git@github.com:bingmann/clone-parsec.git';
$repos{Slate} = 'git@github.com:bingmann/clone-slate.git';

# always acquire montly commits on these projects
my %with_months = (
    "STXXL" => 1,
    "sdsl-lite" => 1,
    "NetworKit" => 1,
    "Glowing-Bear" => 1,
    "DBCSR" => 1,
    "CVC4" => 1,
   );

my %extra_cloc_args = (
    "TuriCreate" => "--exclude-dir=deps",
   );

sub pkg_file($) {
    my ($file) = @_;
    $file = lc($file);
    $file =~ s/\+//g;
    return $file.".json";
}

sub run($) {
    my ($pkg) = @_;
    die("Unknown pkg $pkg") unless defined $repos{$pkg};

    system("rm", "-rf", "/tmp/git-$pkg") and die;
    system("git", "clone", $repos{$pkg}, "/tmp/git-$pkg") and die;

    my $repo = Git->repository (Directory => "/tmp/git-$pkg");

    my $branch = $repo->command('rev-parse', '--abbrev-ref', 'HEAD');
    chomp($branch);

    my @tags = $repo->command('tag');
    #print("$_\n") foreach @revs;

    my @revs = $repo->command('log', '--date=short', '--pretty=format:%ad - %an: %s');
    #print("$_\n") foreach @revs;

    if (@tags < 20 || $with_months{$pkg}) {
        my $lastdate = $revs[-1];
        $lastdate =~ m/^([0-9]{4})-[0-9]{2}-[0-9]{2} / or die;
        my $year = $1;
        $lastdate =~ m/^([0-9]{4}-[0-9]{2}-[0-9]{2}) / or die;
        $lastdate = $1;

        my $firstdate = $revs[0];
        $firstdate =~ m/^([0-9]{4}-[0-9]{2}-[0-9]{2}) / or die;
        $firstdate = $1;

        foreach my $y ($year...2019) {
            foreach my $m (1..12) {
                foreach my $d (1) {
                    my $date = sprintf('%04d-%02d-%02d', $y, $m, $d);
                    next if $date lt $lastdate;
                    next if $date gt $firstdate;
                    my $rev = $repo->command(
                        'rev-list', '-n', '1', '--date=short', "--before=$date", $branch);
                    chomp($rev);
                    push(@tags, $rev);
                    print "$date $rev\n";
                }
            }
        }
        push(@tags, "origin/".$branch);
    }

    my %r;

    foreach my $tag (@tags) {
        print STDERR "Reset to $tag\n";
        eval {
            $repo->command('reset', '--hard', $tag, '--');
        };
        next if $@;

        my $hash = $repo->command('log', '-1', '--pretty=format:%H');

        if (defined $r{$hash}) {
            if ($tag ne $hash) {
                $r{$hash}{tags}{$tag} = 1;
            }
            next;
        }

        my $date = $repo->command('log', '-1', '--date=iso', '--pretty=format:%cd');
        my $ts = $repo->command('log', '-1', '--date=iso', '--pretty=format:%ct');

        my $extra_args = $extra_cloc_args{$pkg} || "";

        open(PIPE, "cloc --quiet --timeout 100 $extra_args --csv /tmp/git-$pkg |")  or die;
        my @langs;
        my $loc;
        while (my $ln = <PIPE>) {
            chomp($ln);
            next if !$ln;
            #print($ln."\n");
            next if $ln =~ /^files,language/;
            my @f = split(/,/, $ln);
            if ($f[1] eq "SUM") {
                $loc = 0 + $f[4];
            }
            else {
                push(@langs, "$f[1]=$f[4]");
            }
        }
        close(PIPE);

        my $langs = join(",", @langs);

        print("$date\t$ts\t$loc\t$tag\t$langs\n");

        $r{$hash}{hash} = $hash;
        $r{$hash}{x} = (0 + $ts) * 1000.0;
        $r{$hash}{y} = 0 + $loc;
        $r{$hash}{date} = $date;
        if ($tag ne $hash) {
            $r{$hash}{tags}{$tag} = 1;
        }
        $r{$hash}{langs} = join("\n", @langs);
    }

    system("rm", "-rf", "/tmp/git-$pkg") and die;

    open(F, ">", pkg_file($pkg)) or die;

    foreach my $h (keys %r) {
        $r{$h}{tags} = join(" ", sort keys %{$r{$h}{tags}});
    }

    my $j = JSON::XS->new->pretty(0)->allow_nonref;
    my $output = $j->encode({
        type => 'line',
        id => $pkg,
        name => $pkg,
        data => [sort({ $$a{x} <=> $$b{x} } values(%r))],
    });
    print $output."\n";

    print(F $output);
    close(F);
}

use List::Util 'shuffle';

if (@ARGV == 0) {
    foreach my $pkg (shuffle keys %repos) {
        next if -e pkg_file($pkg);
        next if -e "/tmp/git-".$pkg;
        run($pkg);
    }
}
else {
    foreach my $a (@ARGV) {
        run($a);
    }
}

