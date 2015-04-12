Find Duplicate Files
====================
Or `fduf` for short, is a command-line tool for finding duplicate files.
`fduf` can either scan only the path given or recursively through
sub-directories.

So, what's the status mister
---------------------------
It works, with the following options available:

- verbose mode, additional info on the scan (filesizes, total number of files
  considered), and
- recursive scanning

The following options are considered for future versions:
- exclude directories with a given name or matching a regular expression, and
- options for formatting, making it easier to redirect output to other CLI
  tools

How does it work
---------------------------
Something along the lines of

    $ fduf [options] [path]

with an example looking like

    $ fduf -r ~/music

The example would search the current user's music stash recursively for duplicate files and output
the result to `stdout`

Since speed is a priority, `fduf` works as follows:

- collect a set of filenames from the path specified (recursively with option -r),
- prune files which have a unique size,
- compute a relatively small and simple checksum for the remaining files,
- prune again files which have a unique checksum,
- compute an md5 hash for the remaining files
- and finally prune files with a unique md5 hash

The resulting set of files is sorted based on size and paths are printed to
`stdout` as mentioned.

The path for _sets_ of duplicate files are printed, seperated by a newline character (`\n`), with each set seperated by an additional newline character, such as

    $ fduf -r ~/music/rhcp
    ~/music/rhcp/some_greatest_album/walkabout.flac
    ~/music/rhcp/one hot minute/walkabout.flac
      
    ~/music/rhcp/some_greatest_album/aeroplane.flac
    ~/music/rhcp/one hot minute/aeroplane.flac
    $
  
<sub>_(Sony, please don't sue me - it's just an example!)_</sub>

So what needs work?
---------------------------
- The makefile,
- the manpage,
- formatting options, and
- option for excluding directories matching some given name(s)
