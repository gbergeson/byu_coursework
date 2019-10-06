#!/usr/bin/python3

import argparse

from packethandler import PacketHandler


def setup():
    """Sets up argument parser and packet handler and returns them."""

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', metavar='protocol', default='TCP', help='Protocol to use in scanning. TCP, UDP and ICMP '
                                                                      'are available. Defaults to TCP.')
    parser.add_argument('-d', metavar='destination', default='127.0.0.1', help='The IPv4 address to be scanned. '
                                                                               'Defaults to 127.0.0.1')
    parser.add_argument('-p', metavar='port(s)', default='0-1000', help='The ports to be scanned. Can be either a '
                                                                        'single number, a comma-separated list (no '
                                                                        'spaces), or a range. Defaults to 0-1000.'
                                                                        'Not used if ICMP connection.')
    parser.add_argument('-t', metavar='timeout', default=10, type=int, help='When to time out the connection in '
                                                                            'seconds. Defaults to 10.')
    parser.add_argument('-o', metavar='output', default='open', help='Output type. "open" prints a summary table of '
                                                                     'open ports, "flags" prints out a table of every '
                                                                     'scanned port with the exact flags returned. '
                                                                     'Defaults to open.')
    parser.add_argument('-f', metavar='filename', default=None, type=str, help='A file of IPs to scan, separated by '
                                                                               'newlines.')
    parser.add_argument('-v', action='store_true', help='Increase verbosity')
    parser.add_argument('-np', action='store_false', help='Don\'t check if host is live before scanning (VERY SLOW)')

    ph = PacketHandler()

    return parser, ph


def main():
    parser, ph = setup()
    args = parser.parse_args()

    # TODO: Enable format checking for arguments.

    if args.f is None:
        dst = args.d
    else:
        with open(args.f) as ip_file:
            dst = ip_file.read().split('\n')[:-1]

    timeout = args.t

    protocol = args.c
    if protocol == 'ICMP':
        ph.send_ping(dst, verbose=True, timeout=timeout)

    else:
        if '-' in args.p:
            start, end = args.p.split('-')
            dport = (int(start), int(end))
        elif ',' in args.p:
            dport = args.p.split(',')
            dport = [int(i) for i in dport]
        else:
            dport = int(args.p)

        output = args.o

        ph.send_packet(protocol, dst, dport, timeout=timeout, output=output, check_live=args.np, verbose=args.v)


if __name__ == '__main__':
    main()
