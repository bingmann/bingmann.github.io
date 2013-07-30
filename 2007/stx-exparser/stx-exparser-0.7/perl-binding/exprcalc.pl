#!/usr/bin/perl -w
# $Id: exprcalc.pl 59 2007-07-17 14:43:23Z tb $
# Test Perl Program for the STX Expression Parser SWIG Wrapper

use lib(".");
use STX::ExpressionParser;

# join command line arguments and parse them
my $pt = STX::ExpressionParser::parseExpression(join(" ", @ARGV));

print "Parsed.toString(): ".$pt->toString()."\n";

# build symbol table and evaulate the parse tree
my $bst = STX::ExpressionParser::BasicSymbolTable->new();
$bst->setVariables({ x => 42, s => "abc", c => "c" });

print "Evaluated: ".$pt->evaluate($bst)->getString()."\n";

