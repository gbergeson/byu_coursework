from scapy.all import IP, TCP, UDP, sr, ICMP


class PacketHandler:
    """Builds, sends, and handles responses to scapy packets."""

    def send_ping(self, dst, verbose=False, timeout=10):
        ans, unans = sr(IP(dst=dst)/ICMP(), timeout=timeout)

        return self.handle_ping_results(ans, unans, verbose=verbose)

    def handle_ping_results(self, ans, unans, verbose):
        up = []
        for sent, received in ans:
            up.append(received.src)

        down = []
        for sent in unans:
            down.append(sent.dst)

        if verbose:
            print('Live hosts:')
            for host in up:
                print(host)

            print('\n')
            print('Dead hosts:')
            for host in down:
                print(host)

        return up, down

    def send_packet(self, protocol, dst, dport, timeout=10, output='open', check_live=True, verbose=False):
        """Builds and sends scapy packets. Prints out the open ports once the scan is complete.

        @:param protocol: The protocol to use in sending the packet. TCP or UDP are accepted.
        @:param dst:      The destination where the packet should be sent. This can be a range. For example,
                          192.168.0.0/24 or google.com/30, or even ['10.10.10.62', '10.10.10.234', ...]
        @:param dport:    The destination port(s) for the packet. Specify ports with a list, or you can input a range
                          with (start, end)
        """

        # TODO: parameter checking. tuple or int for dport, regex stuff for dst.

        if check_live:
            up, down = self.send_ping(dst)
            if verbose:
                print('\nThe following hosts are responsive and will be scanned:')
                for host in up:
                    print(host)
                print('To scan hosts that are not responsive, add the -np (no ping) flag to the command line.\n\n')

            ip = IP(dst=up)
        else:
            ip = IP(dst=dst)

        if protocol == 'TCP':
            prtcl = TCP(dport=dport)

            ans, unans = sr(ip / prtcl, timeout=timeout)
            print('\n')

            if output == 'open':
                self.print_open_table(ans)
            elif output == 'flags':
                self.print_flag_table(ans)
            else:
                raise ValueError('output must be one of "open" or "flags"')

        elif protocol == 'UDP':
            prtcl = UDP(dport=dport)

            ans, unans = sr(ip/prtcl, timeout=timeout)
            print('\n')

            # Method here adapted from Kali Linux Network Scanning Cookbook
            ip_list = [i for i in ip]
            for ip_host in ip_list:
                print(ip_host.dst, 'results:')
                for pkt in unans:
                    if pkt.dst == ip_host.dst:
                        print(pkt[UDP].dport, 'is open')

        else:
            raise ValueError('Only TCP, UDP, and ICMP are currently supported.')



    @staticmethod
    def print_open_table(answers):
        """Prints out a table of IP addresses and ports, with open ports marked with as such."""

        print(answers.filter(lambda sr_tuple: TCP in sr_tuple[1] and sr_tuple[1][TCP].flags & 2)\
                     .make_table(lambda sr_tuple: (sr_tuple[0].dst, sr_tuple[0].dport, "open")))

    @staticmethod
    def print_flag_table(answers):
        """Prints out a table of IP addresses and ports, with the flags returned by each port scanned."""

        print(answers.make_table(lambda sr_tuple:
                                 (sr_tuple[0].dst, sr_tuple[0].dport,
                                  sr_tuple[1].sprintf("{TCP:%TCP.flags%}{ICMP:%IP.src% - %ICMP.type%}")
                                  )
                                 )
              )


'''
    def handle_response(self, interpret=False):
        """Handles a response to a scapy packet.

        @:returns a dictionary with the protocol and flags of the response.

        TODO: If interpret is True, also includes the interpretation of those flags as a
        string. For example, if the response includes the reset flag, the interpretation would
        be "closed".
        """

        if self._response is None:
            protocol = 'No response received.'
            flags = None
        else:
            protocol = self._get_response_protocol()
            flags = self._get_response_flags(protocol)

        response_dict = {
                         'protocol': protocol,
                         'flags': flags
                         }

        if interpret:
            # Do some interpretation things
            interpretation = None
            response_dict['interpretation'] = interpretation

        return response_dict

    def _get_response_protocol(self):
        protocols = ['UDP', 'TCP', 'ICMP']

        for protocol in protocols:
            if self._response.getlayer(protocol) is not None:
                return protocol

        return 'Unknown protocol'

    def _get_response_flags(self, protocol):
        # Thanks, Pierre at https://stackoverflow.com/questions/20429674/get-tcp-flags-with-scapy
        raw_flags = self._response.sprintf('%{}.flags%'.format(protocol))

        flag_codes = {
            'F': 'FIN',
            'S': 'SYN',
            'R': 'RST',
            'P': 'PSH',
            'A': 'ACK',
            'U': 'URG',
            'E': 'ECE',
            'C': 'CWR',
        }

        return [flag_codes[code] for code in raw_flags]
'''
