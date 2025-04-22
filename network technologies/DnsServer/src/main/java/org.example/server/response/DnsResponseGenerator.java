package org.example.server.response;

import org.example.server.request.DiscoverDnsRequest;
import org.example.server.request.DnsRequest;
import org.example.server.request.QueryDnsRequest;
import org.example.server.request.RegisterDnsRequest;
import org.example.util.DnsException;

import java.net.InetAddress;
import java.util.HashMap;

public class DnsResponseGenerator {
    private final InetAddress dnsServerAddress;
    private final HashMap<String, String> domainTable;
    public DnsResponseGenerator(
            InetAddress dnsServerAddress,
            HashMap<String, String> domainTable
    ) {
        this.dnsServerAddress = dnsServerAddress;
        this.domainTable = domainTable;
    }

    public DnsResponse getDnsResponse(DnsRequest request) {
        return switch (request) {
            case DiscoverDnsRequest ignore -> new DiscoverDnsResponse(dnsServerAddress);
            case QueryDnsRequest queryDnsRequest -> new QueryDnsResponse(domainTable.get(queryDnsRequest.domainName()));
            case RegisterDnsRequest registerDnsRequest -> {
                if (domainTable.containsKey(registerDnsRequest.domainName())) {
                    throw new DnsException("Duplicate domain name: " + registerDnsRequest.domainName());
                }
                domainTable.put(registerDnsRequest.domainName(), registerDnsRequest.addressWithPort());
                yield new RegisterDnsResponse(registerDnsRequest.domainName(), registerDnsRequest.addressWithPort());
            }
        };
    }
}
