#!/usr/bin/perl -w
################################################################################
# src/tgf_to_graphml.pl
#
# Convert .tgf "trivial" graph files into .graphml for processing with yEd
################################################################################

use strict;
use warnings;

print <<EOT;
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:y="http://www.yworks.com/xml/graphml" xmlns:yed="http://www.yworks.com/xml/yed/3" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd">
  <!--Created by yEd 3.12.2-->
  <key for="graphml" id="d0" yfiles.type="resources"/>
  <key for="port" id="d1" yfiles.type="portgraphics"/>
  <key for="port" id="d2" yfiles.type="portgeometry"/>
  <key for="port" id="d3" yfiles.type="portuserdata"/>
  <key attr.name="name" attr.type="string" for="node" id="d4"/>
  <key attr.name="url" attr.type="string" for="node" id="d5"/>
  <key attr.name="description" attr.type="string" for="node" id="d6"/>
  <key for="node" id="d7" yfiles.type="nodegraphics"/>
  <key attr.name="weight" attr.type="int" for="edge" id="d8"/>
  <key attr.name="url" attr.type="string" for="edge" id="d9"/>
  <key attr.name="description" attr.type="string" for="edge" id="d10"/>
  <key for="edge" id="d11" yfiles.type="edgegraphics"/>
  <graph edgedefault="directed" id="G">
EOT

my $nodeid = 0;
my $edgeid = 0;

foreach my $file (@ARGV)
{
    open(F, $file) or die;

    my %nodemap;

    while ( my $ln = <F> )
    {
        chomp($ln);
        last if $ln eq "#";

        die unless $ln =~ /^([0-9]+) (.+)$/;

        my ($nlocal, $nlabel) = ($1,$2);

        my $id = $nodeid++;
        $nodemap{$nlocal} = $id;

print <<EOT;
    <node id="n$id">
      <data key="d4"/>
      <data key="d6"/>
      <data key="d7">
        <y:ShapeNode>
          <y:Fill color="#FFCC00" transparent="false"/>
          <y:NodeLabel>$nlabel</y:NodeLabel>
          <y:Shape type="ellipse"/>
        </y:ShapeNode>
      </data>
    </node>
EOT
    }

    while ( my $ln = <F> )
    {
        chomp($ln);
        last unless $ln =~ /^([0-9]+) ([0-9]+) (.+) (red|blue|r|b|br)$/;

        my ($head,$tail,$elocal,$color) = ($1,$2,$3,$4);

        my $id = $edgeid++;

        my $headid = $nodemap{$head};
        my $tailid = $nodemap{$tail};

        defined $headid or die;
        defined $tailid or die;

        $color = "#ff0000" if $color eq "red";
        $color = "#0000ff" if $color eq "blue";
        $color = "#ff0000" if $color eq "r";
        $color = "#0000ff" if $color eq "b";
        $color = "#ff00ff" if $color eq "br";

print <<EOT;
    <edge id="e$id" source="n$headid" target="n$tailid">
      <data key="d10"/>
      <data key="d11">
        <y:PolyLineEdge>
          <y:LineStyle color="$color" type="line" width="2.0"/>
          <y:Arrows source="none" target="none"/>
          <y:EdgeLabel>$elocal</y:EdgeLabel>
          <y:BendStyle smoothed="false"/>
        </y:PolyLineEdge>
      </data>
    </edge>
EOT
    }
}

print <<EOT;
  </graph>
  <data key="d0">
    <y:Resources/>
  </data>
</graphml>
EOT

################################################################################
