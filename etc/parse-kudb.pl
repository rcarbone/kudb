#!/usr/bin/perl
#
# Analyzer for udb files
#
# R. Carbone Jul 2016
#


# print hash
# foreach (keys %data) { print "$_ -> $data{$_}\n"; } print "\n";

my $MAXNAME = 18;

# Format a string of $d characters left aligned
sub fmtstr
{
  my ($str, $d) = @_;
  my $fmt = sprintf ("%%-%d.%ds", $d, $d);

  return sprintf ($fmt, $str);
}

sub rfmtstr
{
  my ($str, $d) = @_;
  my $fmt = sprintf ("%%%d.%ds", $d, $d);

  return sprintf ($fmt, $str);
}


# Format an integer with 'd' digits
sub fmtn
{
  my ($n, $d) = @_;
  my $fmt = sprintf ("%%%dd", $d);

  return sprintf ($fmt, $n);
}


# Put section information in a hash and return its reference
sub fill_data
{
  my ($HANDLE, $line) = @_;
  my %data;

  # handle test name implementation and test type (integer or string)
  if (m/Running test (.+) \(-(.+)\)/) { $data{'name'} = $1; $data{'type'} = $2; }

  # loop over input lines until the end of the section
  while (<$HANDLE>)
    {
      if (m/[benchmark] initial rss: (.+) /)  { $data{'rss_1'}     = $1; }
      elsif (m/# elements: (.+)/)             { $data{'elements'}  = $1; }
      elsif (m/CPU time: (.+) \((.+)/)        { $data{'cpu'}       = $1; }
      elsif (m/[benchmark] rss diff: (.+) /)  { $data{'rss_2'}     = $1; }

      elsif (m/-- CMD: (.+) (.+) (.+) /)      { $data{'name'}      = $2; $data{'type'} = $3; }

      elsif (m/-- totalmem (.+) /)            { $data{'totalmem'}  = $1; $data{'totalmem'}  =~ s/^\s+// }
      elsif (m/-- available (.+) /)           { $data{'available'} = $1; $data{'available'} =~ s/^\s+// }
      elsif (m/-- free (.+) /)                { $data{'free'}      = $1; $data{'free'}      =~ s/^\s+// }

      elsif (m/-- retval (.+)/)               { $data{'retval'}    = $1; $data{'retval'}    =~ s/^\s+// }
      elsif (m/-- real (.+) /)                { $data{'real'}      = $1; $data{'real'}      =~ s/^\s+// }
      elsif (m/-- user (.+) /)                { $data{'user'}      = $1; $data{'user'}      =~ s/^\s+// }
      elsif (m/-- sys (.+) /)                 { $data{'sys'}       = $1; $data{'sys'}       =~ s/^\s+// }
      elsif (m/-- maxrss (.+) /)              { $data{'maxrss'}    = $1; $data{'maxrss'}    =~ s/^\s+// }
      elsif (m/-- avgrss (.+) /)              { $data{'avgrss'}    = $1; $data{'avgrss'}    =~ s/^\s+// }
      elsif (m/-- maxvsize (.+) /)            { $data{'maxvsize'}  = $1; $data{'maxvsize'}  =~ s/^\s+// }
      elsif (m/-- avgvsize (.+) /)            { $data{'avgvsize'}  = $1; $data{'avgvsize'}  =~ s/^\s+// }

      # handle end of section
      last if (defined $data{'avgvsize'});
    }

  # return reference
  return \%data;
}


#
# Handle all the sections included in "Running test " ... "Running test "
#
# Return the array, each element is a hash with relevant data
#
sub parse_file
{
  my ($filename) = @_;

  # array of hashes with relevant data
  my @data = ();
  my $i = 0;

  # Open and parse the file
  open (HANDLE, $filename);

  # loop over input lines until the end of the section
  while (<HANDLE>)
    {
      chomp ($_);

      # start of section
      if (m/Running test/) { @data [$i ++] = fill_data (HANDLE, $_); }

      # handle end of section
      last if (m/Running test/);
    }

  close (HANDLE);

  # return reference
  return \@data;
}


# Print data
sub print_data
{
  my ($type, $sort, @sorted) = @_;
  my $n = 0;

  if ($type eq 'i')
    {
      # Print integer test case results
      if ($sort eq 'cpu')
	{
	  print "Integer test results sorted by CPU Time\n";
	}
      else
	{
	  print "Integer test results sorted by Memory\n";
	}
    }
  else
    {
      # Print string test case results
      if ($sort eq 'cpu')
	{
	  print "String test results sorted by CPU Time\n";
	}
      else
	{
	  print "String test results sorted by Memory\n";
	}
    }

  print "=========================================================\n";
  print "Implementation        | CPU (secs) | Memory (kB) |    #\n";

  $n = 0;
  foreach (@sorted)
    {
      my %data = %$_;       # derefence hash

      if ($data{'type'} eq $type && $data{'cpu'} != 0)
	{
	  my @name = split "/", $data{'name'};
	  print fmtn (++ $n, 2);
	  print "| ";
	  if (@name [0] eq hashit)
	    {
	      my @hit = split "-", @name [1];
	      print "hashit-";
	      print fmtstr (@hit [1], $MAXNAME - 7);
	    }
	  elsif (@name [0] eq "google-c")
	    {
	      my @hit = split "-", @name [1];
	      print "google-c-";
	      print fmtstr (@hit [1], $MAXNAME - 9);
	    }
	  else
	    {
	      print fmtstr (@name [0], $MAXNAME);
	    }
	  print "| ";
	  print rfmtstr ("$data{'cpu'}", 10);
	  print " | ";
	  print rfmtstr ("$data{'maxrss'}", 11);
	  print " | $data{'elements'}";
	  if ($data{'elements'} != 625792)    # old version 624444
	    {
	      print "  Bug here!";
	    }
	  print "\n";
	}
    }
  print "\n";

}


# Sort and print data
sub sort_print_data
{
  my (@sections) = @_;
  my @cpu_sorted = sort { ${$a}{'cpu'}    <=> ${$b}{'cpu'}    } @sections;
  my @mem_sorted = sort { ${$a}{'maxrss'} <=> ${$b}{'maxrss'} } @sections;

  print_data ('i', 'cpu', @cpu_sorted);
  print_data ('s', 'cpu', @cpu_sorted);

  print_data ('i', 'mem', @mem_sorted);
  print_data ('s', 'mem', @mem_sorted);
}


#
# Main
#

# array of files to process
my @files = @ARGV;
die "Error: no input file(s) specified.\n" unless @files;

# input line separator
$/ = "\n";

# Parse each file to collect and print information
foreach $file (@files)
  {
    if (-e $file)
      {
	# Parse file to extract all information of interest
	my $data = parse_file ($file);

	# Print information of interest according to user preferences
	sort_print_data (@$data);
      }
  }

# Bye bye, We are done!
1;
