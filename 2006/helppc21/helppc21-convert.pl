#!/usr/bin/perl -w
#
# HelpPC 2.10 to HTML Converter v1.00 by Timo Bingmann
#
# This Perl script will convert the HelpPC quick reference library by David
# Jurgens into XHTML 1.0 files. The reference library may be old, but many
# parts are still applicable to present-day PCs.
#
# One main goal as to retain the look and feel of the original HelpPC browser
# program, so CSS was used to recreate the original colors and layout as close
# as possible. The script uses Template::Toolkit with embedded text templates
# to output the HTML files. All output files are valid XHTML 1.0 Strict.
#
# Usage:
#
# Download the original helpc21.zip from simtel.net:
#     ftp://ftp.simtel.net/pub/simtelnet/msdos/info/helppc21.zip
#
# Extract the zip file and run the Perl script in the path containing the .TXT
# files. The script will create a subdirectory "html" with 1014 html files.
# You can modify the nostalgic.css or the embedded templates to fit your taste.
#
# Copyright 2006 by Timo Bingmann (tb AT idlebox.net)
# Published on 2006-07-10 under the GNU General Public Licence v2 (GPLv2)
#

use strict;
use warnings;

use Template;

# helppc txt files are dos style
$/ = "\r\n";

### Input functions

# global data

my @filetitles;		# contains the files and their titles
my @nodelist;		# contains all text nodes
my %keywords;		# contains all node keywords associated with the number
                        # of the node
my %keywordslc;		# contains all node keywords in lower case. used for
                        # duplicate checking
my $currnode = -1;

# calculate the file name of a node: removes illegal characters
sub calcfilename {
    my ($node) = @_;

    my $title = $$node{title};
    $title =~ tr/ ,/__/;
    $title =~ s/[\(\).]//g;

    return $$node{file}."-".$title.".html";
}

# find the node referenced by a keyword
sub makekeywordlink {
    my ($kwbase) = @_;

    foreach my $kw ($kwbase, $kwbase."s", "_".$kwbase)
    {
	my $idx = $keywordslc{lc $kw};
	if (defined $idx) {
	    return calcfilename( $nodelist[$idx] );
	}
    }

    # check if only one keyword is a substring match
    my @submatch = grep({ index(lc($_), lc($kwbase)) >= 0 } keys %keywordslc);

    if (scalar(@submatch) == 1) {
	return calcfilename( $nodelist[ $keywordslc{$submatch[0]} ] );
    }
    print(STDERR "missing cross-reference keyword: $kwbase\n");

    return "";
}


# character from cp437 to unicode entitiy transformation map
my @charmap =
(
# 0/8     1/9     2/A     3/B     4/C     5/D     6/E     7/F
 0x0020, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022, # 0
 0x25D8, 0x25E6, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C, # 8
 0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8, # 10
 0x2191, 0x2193, 0x2192, 0x2190, 0x2310, 0x2194, 0x25B2, 0x25BC, # 18
 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, # 20
 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, # 28
 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, # 30
 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, # 38
 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, # 40
 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, # 48
 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, # 50
 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, # 58
 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, # 60
 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, # 68
 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, # 70
 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0394, # 78
 0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7, # 80
 0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5, # 88
 0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9, # 90
 0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192, # 98
 0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA, # A0
 0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB, # A8
 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, # B0
 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510, # B8
 0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F, # C0
 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567, # C8
 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B, # D0
 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580, # D8
 0x03B1, 0x03B2, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x03BC, 0x03C4, # E0
 0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x2205, 0x2208, 0x2229, # E8
 0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248, # F0
 0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0, # F8
);

# convert one line of ascii text into html. add hyperlinks for ~title~
sub transformline {
    my ($in, $hyperref) = @_;

    my $out = "";
    my $col = 0;	# count column to calculate tabs
    my $ws = 0;		# count number of consecutive whitespaces

    my $inref = 0;
    my $reftext;

    my %htmlescape = ("&" => "&amp;", "<" => "&lt;", ">" => "&gt;");

    foreach my $l (split(//,$in))
    {

	if ($hyperref and $l eq "~") # beginning or end of a hyperlink
	{
	    if (!$inref) { # beginning
		$inref = 1;
		$reftext = "";
	    }
	    else { # end
		if ($reftext eq "") { # was a ~~
		    $out .= "~";
		    $col++;
		}
		else {
		    $out .= sprintf('<a href="%s">%s</a>',
				    makekeywordlink($reftext), $reftext);
		    $col += length($reftext);
		}
		$inref = 0;
	    }
	}
	elsif ($inref) {
	    $reftext .= $l;
	    next;
	}
	elsif ($l eq "\t")
	{
	    # expand tab.
	    $out .= " " 	if $ws % 2 == 0;
	    $out .= "&nbsp;" 	if $ws % 2 == 1;
	    $col++;
	    $ws++;

	    while($col % 8 != 0) {
		$out .= " " 	 if $ws % 2 == 0;
		$out .= "&nbsp;" if $ws % 2 == 1;
		$col++;
		$ws++;
	    }
	}
	elsif ($l eq " ")
	{
	    $out .= " " 	if $ws % 2 == 0;
	    $out .= "&nbsp;" 	if $ws % 2 == 1;

	    $col++;
	    $ws++;
	}
	elsif (defined $htmlescape{$l})
	{
	    $out .= $htmlescape{$l};
	    $col++;
	    $ws = 0;
	}
	elsif (ord($l) == $charmap[ord($l)])
	{
	    $out .= $l;
	    $col++;
	    $ws = 0;
	}
	else {
	    $out .= sprintf("&#%d;",$charmap[ord($l)]);
	    $col++;
	    $ws = 0;
	}
    }
    return $out;
}

# convert a whole text node into html. handles centering for lines beginning
# with % or ^
sub transformtext {
    my ($intext) = @_;

    my $out = "";

    foreach my $ln (split(/\n/, $intext))
    {
	my $firstchar = substr($ln, 0, 1);
	$ln = (length($ln) > 0) ? substr($ln, 1) : "";

	my $highlight = 0;

	# just text
	if ($firstchar eq "%") { # highlight line
	    $highlight = 1;
	}
	elsif ($firstchar eq "^") { # center and highlight line
	    # center to 80 chars per line

	    $ln = (" " x int((80 - length($ln))/2)) . $ln;

	    $highlight = 1;
	}
	else {
	    $ln = $firstchar.$ln;
	}

	$out .= '<span class="highlight">' if $highlight;
	$out .= transformline($ln, 1);
	$out .= '</span>' if $highlight;
	$out .= "<br />\n";
    }

    return $out;
}

# load one of the helppc txt files into the global data
sub loadtxt {
    my ($filename, $fileshort) = @_;

    open(F, $filename) or die("Could not open $filename: $!");

    my $filetitle;

    my %node;

    while( my $ln = <F> )
    {
	chomp($ln);

	my $firstchar = substr($ln, 0, 1);
	$ln = (length($ln) > 0) ? substr($ln, 1) : "";

	if ($firstchar eq "@")
	{
	    print(STDERR "loading file: $ln\n");
	    $filetitle = $ln;

	    push(@filetitles, { file => $fileshort, title => $filetitle });
	}
	elsif ($firstchar eq ":")
	{
	    my @keywords = split(/:/, $ln);

	    print(STDERR "node title: $keywords[0]\n");
	    #print(STDERR "node keywords: ".join(" = ", @keywords)."\n");

	    # save current node
	    if ($node{title}) {
		push(@nodelist, {%node});
	    }

	    $currnode++;
	    $currnode == scalar(@nodelist) or die();

	    foreach my $kw (@keywords) {
		if (defined $keywordslc{lc $kw}) {
		    print(STDERR "duplicate keywords: $kw\n");
		    next;
		}
		$keywords{$kw} = scalar(@nodelist);
		$keywordslc{lc $kw} = scalar(@nodelist);
	    }

	    %node =
		(
		 title => $keywords[0],
		 keywords => [@keywords],
		 file => $fileshort,
		 text => "",
		);
	}
	else
	{
	    $node{text} .= $firstchar.$ln."\n";
	}
    }

    if ($node{title}) {
	push(@nodelist, {%node});
    }

    close(F);

    return $filetitle;
}

### Output functions using Template::Toolkit

# base tt layout used as WRAPPER
my $ttlayout = <<EOF;
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <meta content="text/html; charset=ISO-8859-1" http-equiv="content-type" />
  <title>HelpPC 2.10 - [% title %]</title>
  <link type="text/css" rel="stylesheet" href="nostalgic.css" title="nostalgic" />
</head>
<body>
<!-- Converted from the original text files by helppc21-convert.pl by Timo Bingmann - http://idlebox.net/ -->
<table class="bodytable" summary="">
  <tr class="top">
    <td class="unimono" style="text-align: left; width: 30%"><a class="menuplain" href="index.html">HelpPC 2.10</a></td>
    <td class="unimono" style="text-align: center; width: 30%"><a class="menuplain" href="index.html">Quick Reference Utility</a></td>
    <td class="unimono" style="text-align: right; width: 40%">Copyright 1991 David Jurgens</td>
  </tr>
  <tr>
    <td colspan="3">
      <table style="margin-left: auto; margin-right: auto" summary="">
        <tr>
          <td class="unimono">
            <br />[% content  %]<br />
          </td>
        </tr>
      </table>
    </td>
  </tr>

  <tr class="bottom">
    <td class="unimono" style="text-align: left; width: 30%">Esc or Alt-X to exit</td>
    <td class="unimono" style="text-align: center; width: 40%">[% title %]</td>
    <td class="unimono" style="text-align: right; width: 30%">Home/PgUp/PgDn/End &#8592;&#8593;&#8595;&#8594;</td>
  </tr>
</table>
<div class="converter"><a href="http://idlebox.net/2006/helppc21/">Converted to HTML in 2006 by Timo Bingmann</a></div>
</body>
</html>
EOF

# reuse the same template object
my $templateproc = Template->new({ TRIM => 1,
				   PRE_CHOMP => 1,
				   POST_CHOMP => 1,
				   BLOCKS => {
					      ttlayout => $ttlayout
					     },
				 });

# write index.html containing the file titles
sub writeindex {

my $tttext = <<EOF;
[% WRAPPER ttlayout title = 'Choose Topic Category' %]
<br /><br /><br />
<span class="highlight"> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Main Topic Menu</span><br />
<br />
[% FOREACH file = filelist %]
<a class="menulink" href="[% file.link %]">[% file.title %]</a><br />
<br />
[% END %]
<br /><br /><br /><br />
[% END %]
EOF

    # add the link to the converted HELPPC.DOC on the index page
    my $helppcdocentry = {
			  file => "helppcdoc",
			  title => "HELPPC.DOC Readme",
			 };
    my @filelist;

    foreach my $f (@filetitles, $helppcdocentry)
    {
	push(@filelist,
	     {
	      title => "&nbsp;".transformline($$f{title}.(" " x (40 - length($$f{title})))),
	      link => "$$f{file}.html",
	     }
	    );
    }

    my $vars = { filelist => \@filelist };

    $templateproc->process(\$tttext, $vars, "html/index.html")
	or die $templateproc->error();
}

# write a node index file for each txt contain the keywords
sub writefileindex {
    my ($filehash) = @_;

my $tttext = <<EOF;
[% WRAPPER ttlayout title = 'Choose Subtopic' %]
[% title %]
<br />
[% FOREACH topic = topiclist %]
  <a class="menulink" href="[% topic.link %]">[% topic.title %]</a>
  [% IF loop.index() % 3 != 2 %]
&nbsp;&nbsp;
  [% ELSE %]
<br />
  [% END %]
[% END %]
<br />
[% END %]
EOF

    # this creates the link's text: some magic to get the hover bars right
    sub titleconvert {
	my ($t) = @_;

	(length $t > 20) and $t = substr($t, 1, 20);
	$t .= (" " x (20 - length($t)));

	return "&nbsp;".transformline($t);
    }

    my @topiclist;

    foreach my $key (sort { lc($a) cmp lc($b) } keys %keywords)
    {
	my $nodeid = $keywords{$key};
	next if $nodelist[$nodeid]{file} ne $$filehash{file};

	push(@topiclist,
	     {
	      title => titleconvert($key),
	      link => calcfilename($nodelist[$nodeid])
	     }
	    );
    }

    my $vars =
    {
     title => transformtext("^".$$filehash{title}),
     topiclist => \@topiclist
    };

    $templateproc->process(\$tttext, $vars, "html/$$filehash{file}.html")
	or die $templateproc->error();
}

# write a node text file. quite straight forward
sub writenode {
    my ($node) = @_;

    my $fn = calcfilename($node);

my $tttext = <<EOF;
[% WRAPPER ttlayout title=node.title %]
[% node.text %]
[% END %]
EOF

    my $vars = { node => $node };

    $templateproc->process(\$tttext, $vars, "html/$fn")
	or die $templateproc->error();
}

# write a plain text file. pipe it through transformline
sub writeplaintext {
    my ($infile, $outfile, $title) = @_;

    open(IF, $infile)
	or die("Could not open plain text file $infile: $!");

    my $text = "";
    my $firsttitle = 1;

    while( my $ln = <IF> ) {
	chomp($ln);

	# remove page feeds and page titles
	($ln =~ /^\s*\x0C\s*$/) and next;

	if ($ln =~ /HelpPC 2\.10 \s+ Quick Reference Utility \s+ Copyright \(c\) 1991, David Jurgens/)
	{
	    if ($firsttitle) { $firsttitle = 0; } # write it the first time
	    else { next; }
	}

	$text .= transformline($ln, 0)."<br />\n";
    }
    close(IF);

my $tttext = <<EOF;
[% WRAPPER ttlayout title=title %]
[% text %]
[% END %]
EOF

    my $vars = { title => $title, text => $text };

    $templateproc->process(\$tttext, $vars, "html/$outfile")
	or die $templateproc->error();
}

# write the nostalgic.css into the html dir
sub writecss {

    open(F, "> html/nostalgic.css")
	or die("Could not open html/nostalgic.css: $!");

my $css = <<EOF;

body { color: white; background: black; margin: 0px }

table.bodytable { width: 100%; border-collapse: collapse; color: #111111; background: black }

table.bodytable tr.top td { background: #0000AC; color: #FFFFFF; font-weight: bold }

table.bodytable tr.bottom td { background: #0000AC; color: #FFFFFF; font-weight: bold }

.unimono { font-family: monospace; font-size: 12pt; letter-spacing: 0; line-height: 11.9pt; color: #dddddd }

.unimono span.highlight { color: #52FFFF; font-weight: bold; text-decoration: none }

.unimono a { color: #ffff52; text-decoration: none }

.unimono a:hover { color: #5255FF; background: #ACAAAC }

.unimono a.menuplain { color: #ffffff; text-decoration: none }

.unimono a.menuplain:hover { color: #0000AC; background: #FFFFFF }

.unimono a.menulink { color: #dddddd; text-decoration: none }

.unimono a.menulink:hover { color: #ffffff; background: #0000AC; font-weight: bold; }

div.converter { text-align: right; font-family: monospace; font-size: 9pt; padding-top: 0.3ex }

div.converter a { color: #2222FF; text-decoration: none }

div.converter a:visited { color: #2222FF }

EOF

    print(F $css);

    close(F);
}

### Main Program begins here

## load each of the following helppc format txt files

my @scanfiles =
(
 ["ASM.TXT"		=> "asm"],
 ["C.TXT"		=> "c"],
 ["HARDWARE.TXT"	=> "hw"],
 ["INTERRUP.TXT"	=> "int"],
 ["TABLES.TXT"		=> "table"],
 ["MISC.TXT"		=> "misc"],
);

foreach my $f (@scanfiles) {
    loadtxt($$f[0], $$f[1]);
}

# check that node filenames are unique
my %fnlist = map({ (lc(calcfilename($_)) => 1) } @nodelist);

if (scalar(keys(%fnlist)) != scalar(@nodelist)) {
    die("Duplicate filenames!");
}

# calculate text transformation and hyperlinks
foreach my $n (@nodelist) {
    $$n{text} = transformtext($$n{text});
}

## write the files

writeindex();
writecss();

writeplaintext("HELPPC.DOC", "helppcdoc.html", "HelpPC Readme");

foreach my $f (@filetitles) {
    writefileindex($f);
}

foreach my $n (@nodelist) {
    writenode($n);
}

print(STDERR "All done.\n");

__END__
