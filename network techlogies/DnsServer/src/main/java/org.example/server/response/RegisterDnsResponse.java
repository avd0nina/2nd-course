package org.example.server.response;

public record RegisterDnsResponse(
        String domainName,
        String address
) implements DnsResponse {

    public RegisterDnsResponse {

    }

    @Override
    public byte[] bytes() {
        return "REGISTERED %s -> %s".formatted(domainName, address).getBytes();
    }
}
