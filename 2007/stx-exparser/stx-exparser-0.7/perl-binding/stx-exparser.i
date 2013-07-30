/* $Id: stx-exparser.i 59 2007-07-17 14:43:23Z tb $ */

/*
 * STX Expression Parser C++ Framework v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

%module "STX::ExpressionParser"
%{
#include "AnyScalar.h"
#include "ExpressionParser.h"

using namespace stx;
%}

%include "std_string.i"
%include "std_vector.i"

%exception {
    try {
        $action
    }
    catch (ExpressionParserException &e) {
        croak(e.what());
    }
}

%include "AnyScalar.h"
%include "ExpressionParser.h"

%perlcode %{
package STX::ExpressionParser::BasicSymbolTable;

# Nicer Function to add a hash of variables to the symbol table. Directly calls
# the constructor new($$hashref{$k})
sub setVariables($\%) {
    my ($self,$hashref) = (@_);

    foreach my $k (keys %$hashref) {
	my $v = STX::ExpressionParser::AnyScalar->new();
	$v->setAutoString("$$hashref{$k}");
	$self->setVariable($k, $v);
    }
}

%}