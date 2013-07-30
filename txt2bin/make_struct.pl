#!/usr/bin/perl
use strict;
use warnings;

our @full_types; 
our @strs;

my $index=1;

my $data    = $ARGV[0];
my $struct_name = $ARGV[1];
my $which = $ARGV[2];
open FILE, "<", $data or die $!;

my @data = <FILE>;

my $tmp = $data[0];
chomp($tmp);
@strs = split(" ",$tmp);

$tmp = $data[1];
chomp($tmp);
my @types = split(" ",$tmp);

close FILE;

&getType($_) for (@types);

if ($which =~ m/h/i) {
&printLoaderH();
}
else {
&printLoaderCPP();
}

sub getType {
   my $type = $_[0]; 
push @full_types, "float"        if ($type =~ m/f/i);
push @full_types, "int"          if ($type =~ m/i/i);
push @full_types, "char"         if ($type =~ m/c/i);
push @full_types, "double"       if ($type =~ m/d/i);
push @full_types, "long"         if ($type =~ m/l/i);
push @full_types, "unsigned int" if ($type =~ m/ui/i);
}

sub printLoaderCPP {
print "#include <fstream>\n";
print "\n";
print "#include \"glLoader.h\"\n";
print "#include \"gl" . $struct_name . "Loader.h\"\n";
print "\n";
print "gl" . $struct_name . "Loader::gl" . $struct_name . "Loader(char * filename) {\n";
print "    f_size = glLoader::getFileLength(filename)/sizeof(gl" . $struct_name . ");\n";
print "    loadFile(filename, f_size);\n";
print "}\n";
print "\n";
print "gl" . $struct_name . "Loader::gl" . $struct_name . "& gl" . $struct_name . "Loader::operator[] (const unsigned int i) {\n";
print "    return mem[i];\n";
print "}\n";
print "\n";
print "gl" . $struct_name . "Loader::gl" . $struct_name . "* gl" . $struct_name . "Loader::data() {\n";
print "    return mem;\n";
print "}\n";
print "\n";
print "unsigned int gl" . $struct_name . "Loader::length() {\n";
print "    return f_size;\n";
print "}\n";
print "\n";
print "unsigned int gl" . $struct_name . "Loader::bytes() {\n";
print "    return f_size * sizeof(gl" . $struct_name . ");\n";
print "}\n";
print "\n";
print "void gl" . $struct_name . "Loader::loadFile(char* filename, unsigned int size) {\n";
print "    mem = new gl" . $struct_name . "[size];\n";
print "    std::ifstream IN(filename, std::ios_base::in);\n";
print "\n";
print "    if(!IN.good()){\n";
print "        f_size = 0;\n";
print "    }\n";
print "\n";
print $full_types[$_] . " " . $strs[$_] . ";\n" for (0..$#full_types);
print "    int i = 0;\n";
print "\n";
print "    while(IN"; 
print " >> " . $strs[$_] for (0..$#strs); 
print ") {\n";
print "       mem[i]." . $strs[$_] . " = " . $strs[$_] . ";\n" for (0..$#strs);
print "       i++;\n";
print "    }\n";
print "    IN.close();\n";
print "}\n";
}

sub printLoaderH {
print "#ifndef __gl". $struct_name . "Loader_H__\n";
print "#define __gl". $struct_name . "Loader_H__\n";
print "\n";
print "class gl". $struct_name . "Loader  {\n";
print "\n";
print "public:\n";
print "   typedef struct {\n";
print $full_types[$_] . " " . $strs[$_] . ";\n" for (0..$#full_types);
print "   } gl" . $struct_name . ";\n";
print "\n";
print "  gl" . $struct_name . "Loader(char * filename);\n";
print "\n";
print "  gl" . $struct_name . "* data      ();\n";
print "  gl" . $struct_name . "& operator[](unsigned int i);\n";
print "\n";
print "  unsigned int length ();\n";
print "  unsigned int bytes  ();\n";
print "protected:\n";
print "\n";
print "\n";
print "   unsigned int f_size;\n";
print "   gl" . $struct_name . " * mem;\n";
print "\n";
print "   void loadFile(char* filename, unsigned int size);\n";
print "};\n";
print "\n";
print "#endif\n";
}
