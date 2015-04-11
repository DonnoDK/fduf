Find Duplicate Files
====================
Or `fduf` for short, is a command-line tool for finding duplicate files.
`fduf` can either scan only the path given or recursively through
sub-directories.

So, what's the status mister
---------------------------
It works, but there are no logic for parsing additional command-line arguments.
The following are in the works however:

- verbose mode, additional info on the scan (filesizes, count on files
  considered),
- exclude directories with a given name or matching a regular expression,
- options for formatting, making it easier to redirect output to other CLI
  tools

How does it work
---------------------------
Ehm, it's not set in stone yet, but something along the lines of

    $ fduf [path]

works at the moment. As an example

    $ fdup .

would search the current directory recursively for duplicate files and output
the result to `stdout`

Since speed is a priority, `fduf` works as follows:

- collect a set of filenames from the path specified (recursively by default),
- prune files which have a unique size,
- compute a relatively small and simple checksum for the remaining files,
- prune again files which have a unique checksum,
- compute an md5 hash for the remaining files
- and finally prune files with a unique md5 hash

The resulting set of files is sorted based on size and paths are printed to
`stdout` as mentioned.

So what's missing?
---------------------------
Argument options and a proper makefile. Pull requests are welcome.
