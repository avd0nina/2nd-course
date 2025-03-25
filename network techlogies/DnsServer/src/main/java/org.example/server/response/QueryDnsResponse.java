package org.example.server.response;

import org.example.util.DnsException;

public record QueryDnsResponse(String ip) implements DnsResponse {

    public QueryDnsResponse {
        if (ip == null) {
            throw new DnsException("address not found");
        }
    }

    @Override
    public byte[] bytes() {
        return ip.getBytes();
    }
}
