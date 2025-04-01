package org.example.server.request;

public record RegisterDnsRequest(
        String domainName,
        String addressWithPort
) implements DnsRequest {
}
