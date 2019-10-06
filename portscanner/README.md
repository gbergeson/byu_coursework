A basic port scanner. Sends ICMP pings and can perform TCP or UDP scans on a wide range of ports. 

```
portscanner.py --help
usage: portscanner.py [-h] [-c protocol] [-d destination] [-p ports]
                      [-t timeout] [-o output] [-f filename] [-v] [-np]

optional arguments:
  -h, --help      show this help message and exit
  -c protocol     Protocol to use in scanning. TCP, UDP and ICMP are
                  available. Defaults to TCP.
  -d destination  The IPv4 addresses to be scanned. Defaults to 127.0.0.1
  -p port(s)      The ports to be scanned. Can be either a single number, a
                  comma-separated list (no spaces), or a range. Defaults to
                  0-1000. Not used if ICMP is specified.
  -t timeout      When to time out the connection in seconds. Defaults to 10.
  -o output       Output type. "open" prints a summary table of open ports,
                  "flags" prints out a table of every scanned port with the
                  exact flags returned. Defaults to open. Only used for TCP.
  -f filename     A file of IPs to scan, separated by newlines.
  -v              Increase verbosity
  -np             Don't check if hosts are live before scanning (VERY SLOW)
  ```
  Destination can be specified in the following formats: 192.168.0.1, 192.168.0.0/24, or google.com/30.
  Multiple ports can be specified in a hyphen-separated range.
  
