package org.example.server.request;

import org.example.util.DnsException;

import java.net.DatagramPacket;

public sealed interface DnsRequest permits
        DiscoverDnsRequest,
        RegisterDnsRequest,
        QueryDnsRequest
{

    static DnsRequest convert(DatagramPacket requestPacket) {
        String request = new String(requestPacket.getData(), 0, requestPacket.getLength());
        String[] parts = request.split(" ");
        return switch (parts[0]) {
            case "DISCOVER_DNS" -> new DiscoverDnsRequest();
            case "REGISTER" -> {
                if (parts.length < 3) {
                    throw new DnsException("ERROR Invalid REGISTER format");
                }
                yield new RegisterDnsRequest(parts[1], parts[2]);
            }
            case "QUERY" -> {
                if (parts.length < 2) {
                    throw new DnsException("ERROR Invalid QUERY format");
                }
                yield new QueryDnsRequest(parts[1]);
            }
            default -> throw new DnsException("ERROR Unknown command");
        };
    }
}
