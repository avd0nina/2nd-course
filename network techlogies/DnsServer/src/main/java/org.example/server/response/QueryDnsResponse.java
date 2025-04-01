package org.example.server.response;

import org.example.util.DnsException;

public record QueryDnsResponse(String addressWithPort) implements DnsResponse {

    public QueryDnsResponse {
        if (addressWithPort == null) {
            throw new DnsException("address not found");
        }
    }

    @Override
    public byte[] bytes() {
        return addressWithPort.getBytes();
    }
}
