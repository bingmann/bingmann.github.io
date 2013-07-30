# $Id: Makefile.am 155 2006-05-24 22:57:29Z bingmann $

SUBDIRS = . tools testsuite

bin_PROGRAMS = test1

test1_SOURCES = test1.cc

test1_LDADD = libvgserver.a

noinst_LIBRARIES = libvgserver.a libvgclient.a

libvgserver_a_SOURCES = Graph.h GraphTypes.h GraphParser.h GraphClientFace.h  GraphPort.h ByteBuffer.h RTree.h TpArray.h AttributeBlob.h AttributeBlob_fwd.h AttributeBlob_impl.h GraphData.cc GraphData.h Changelist.cc Changelist.h ChangeTimeline.cc ChangeTimeline.h GraphContainer.cc GraphContainer.h GraphConnection.cc GraphConnection.h GraphServer.cc GraphServer.h ByteOutBuffer.cc ByteOutBuffer.h ByteInBuffer.cc ByteInBuffer.h AnyType.cc AnyType.h AttributeProperties.cc AttributeProperties.h GraphProperties.h GraphPropertiesParser.cc GraphLoader.cc GraphLoader.h GraphLoaderKFile.h AttributeParser.cc AttributeParser.h

libvgclient_a_SOURCES = AnyType.cc AnyType.h AttributeProperties.cc AttributeProperties.h ByteInBuffer.cc ByteInBuffer.h

AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall -Wold-style-cast
