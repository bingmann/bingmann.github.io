<?xml version="1.0" encoding="iso-8859-1"?>

<!--
   svn2html.xsl - xslt stylesheet for converting svn log to a normal
                  changelog fromatted in html

   Usage (replace ++ with two minus signs):
     svn ++verbose ++xml log | \
       xsltproc ++stringparam strip-prefix `basename $(pwd)` \
                ++stringparam groupbyday yes \
                svn2html.xsl - > ChangeLog.html

   This file is partially based on svn2cl.xsl.

   Copyright (C) 2005 Arthur de Jong.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. The name of the author may not be used to endorse or promote
      products derived from this software without specific prior
      written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

<!DOCTYPE page [
 <!ENTITY tab "&#9;">
 <!ENTITY newl "&#13;">
 <!ENTITY space "&#32;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

 <xsl:output
   method="xml"
   encoding="iso-8859-1"
   media-type="text/html"
   omit-xml-declaration="no"
   standalone="yes"
   indent="yes"
   doctype-public="-//W3C//DTD XHTML 1.1//EN"
   doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd" />

 <xsl:strip-space elements="*" />

 <!-- the prefix of pathnames to strip -->
 <xsl:param name="strip-prefix" select="'/'" />

 <!-- whether entries should be grouped by day -->
 <xsl:param name="groupbyday" select="'no'" />

 <!-- match toplevel element -->
 <xsl:template match="log">
  <html>
   <head>
    <title>Algebra 1 Mitschrift WS05/06 bei Prof. Herrlich</title>
    <link rel="stylesheet" href="a1.css" type="text/css" />
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
   </head>
   <body>

    <h1>Algebra 1 Mitschrift WS05/06</h1>

    <h2>Info</h2>

    <p>Hier gibts meine geTeXte Mitschrift zur Vorlesung Algebra 1 bei Prof. Herrlich im Wintersemester 2005/06.</p>
    <p>Nur weil die Mitschrift so schöner aussieht, ist diese nicht richtiger. Im Gegenteil, durch TeXen schleichen sich mehr Fehler in das Skript: bitte schickt mir Hinweise zu Fehlern im Skript.</p>

    <h2>Download</h2>
    <p><a href="Algebra1.pdf">Algebra1.pdf</a> (<xsl:value-of select="$PDFSIZE"/> kB) - kompiliertes PDF</p>
    <p><a href="Algebra1.zip?download">Algebra1.zip</a> (<xsl:value-of select="$ZIPSIZE"/> kB) - gezippte Sources (<a href="Algebra1.zip/">browse zip</a>)</p>

    <h2>Änderungen</h2>
    <table class="svn_table">
     <xsl:apply-templates />
    </table>

    <p></p>
   </body>
  </html>
 </xsl:template>

 <!-- format one entry from the log -->
 <xsl:template match="logentry">
  <!-- save log entry number -->
  <xsl:variable name="pos" select="position()" />
  <!-- fetch previous entry's date -->
  <xsl:variable name="prevdate">
   <xsl:apply-templates select="../logentry[position()=(($pos)-1)]/date" />
  </xsl:variable>
  <!-- fetch previous entry's author -->
  <xsl:variable name="prevauthor">
   <xsl:apply-templates select="../logentry[position()=(($pos)-1)]/author" />
  </xsl:variable>
  <!-- fetch this entry's date -->
  <xsl:variable name="date">
   <xsl:apply-templates select="date" />
  </xsl:variable>
 
  <!-- output one line in the table -->
  <tr>
   <td>
   <!-- get revision number -->
   <span class="changelog_revision">r<xsl:value-of select="@revision" /></span>
   </td>
   <td>
   <span class="changelog_date"><xsl:apply-templates select="date" /></span>
   </td>
   <td>
    <span class="changelog_message">
     <!-- TODO: translate line breaks to <br /> tags -->
     <xsl:value-of select="msg" />
    </span>
   </td>
  </tr>
 </xsl:template>

 <!-- format date -->
 <xsl:template match="date">
  <xsl:variable name="date" select="normalize-space(.)" />
  <!-- output date part -->
  <xsl:value-of select="substring($date,1,10)" />
  <!-- output time part -->
  <xsl:if test="$groupbyday!='yes'">
   <xsl:text>&space;</xsl:text>
   <xsl:value-of select="substring($date,12,5)" />
  </xsl:if>
 </xsl:template>

 <!-- format author -->
 <xsl:template match="author">
  <xsl:value-of select="normalize-space(.)" />
 </xsl:template>

 <!-- present a list of paths names -->
 <xsl:template match="paths">
  <xsl:for-each select="path">
   <xsl:sort select="normalize-space(.)" data-type="text" />
   <!-- unless we are the first entry, add a comma -->
   <xsl:if test="not(position()=1)">
    <xsl:text>,&space;</xsl:text>
   </xsl:if>
   <!-- print the path name -->
   <xsl:apply-templates select="." />
  </xsl:for-each>
 </xsl:template>

 <!-- transform path to something printable -->
 <xsl:template match="path">
  <!-- fetch the pathname -->
  <xsl:variable name="p1" select="normalize-space(.)" />
  <!-- strip leading slash -->
  <xsl:variable name="p2">
   <xsl:choose>
    <xsl:when test="starts-with($p1,'/')">
     <xsl:value-of select="substring($p1,2)" />
    </xsl:when>
    <xsl:otherwise>
     <xsl:value-of select="$p1" />
    </xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <!-- strip trailing slash from strip-prefix -->
  <xsl:variable name="sp">
   <xsl:choose>
    <xsl:when test="substring($strip-prefix,string-length($strip-prefix),1)='/'">
     <xsl:value-of select="substring($strip-prefix,1,string-length($strip-prefix)-1)" />
    </xsl:when>
    <xsl:otherwise>
     <xsl:value-of select="$strip-prefix" />
    </xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <!-- strip strip-prefix -->
  <xsl:variable name="p3">
   <xsl:choose>
    <xsl:when test="starts-with($p2,$sp)">
     <xsl:value-of select="substring($p2,1+string-length($sp))" />
    </xsl:when>
    <xsl:otherwise>
     <!-- TODO: do not print strings that do not begin with strip-prefix -->
     <xsl:value-of select="$p2" />
    </xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <!-- strip another slash -->
  <xsl:variable name="p4">
   <xsl:choose>
    <xsl:when test="starts-with($p3,'/')">
     <xsl:value-of select="substring($p3,2)" />
    </xsl:when>
    <xsl:otherwise>
     <xsl:value-of select="$p3" />
    </xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <!-- translate empty string to dot -->
  <xsl:choose>
   <xsl:when test="$p4 = ''">
    <xsl:text>.</xsl:text>
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="$p4" />
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>

</xsl:stylesheet>
