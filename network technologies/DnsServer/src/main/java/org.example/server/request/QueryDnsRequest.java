package org.example.server.request;

import org.example.util.DnsException;

public record QueryDnsRequest(String domainName) implements DnsRequest {

    public QueryDnsRequest {
        if (domainName == null) {
            throw new DnsException("Domain name cannot be null");
        }
    }
}
