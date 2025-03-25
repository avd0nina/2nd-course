package org.example.server.request;

public record RegisterDnsRequest(
        String domainName,
        String address
) implements DnsRequest {
}
