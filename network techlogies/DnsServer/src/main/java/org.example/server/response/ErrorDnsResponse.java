package org.example.server.response;

import org.example.util.DnsException;

public record ErrorDnsResponse(DnsException exception) implements DnsResponse {

    @Override
    public byte[] bytes() {
        return exception.getMessage().getBytes();
    }
}
