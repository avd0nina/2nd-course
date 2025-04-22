package org.example.server;

import org.example.server.request.DnsRequest;
import org.example.server.response.DnsResponse;
import org.example.server.response.DnsResponseGenerator;
import org.example.server.response.ErrorDnsResponse;
import org.example.util.DnsException;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.HashMap;

public class DnsServer {
    private static final int PORT = 5354;
    public static DatagramPacket createDatagramPacket(
            DatagramPacket requestPacket,
            DnsResponse dnsResponse
    ) {
        return new DatagramPacket(
                dnsResponse.bytes(),
                dnsResponse.bytes().length,
                requestPacket.getAddress(),
                requestPacket.getPort()
        );
    }

    public static void main(String[] args) throws IOException {
        try (DatagramSocket socket = new DatagramSocket(PORT)) {
            System.out.println("DNS Server is running on port " + PORT);

            DnsResponseGenerator dnsResponseGenerator = new DnsResponseGenerator(
                    InetAddress.getLocalHost(),
                    new HashMap<>()
            );

            while (true) {
                byte[] buffer = new byte[512];
                DatagramPacket requestPacket = new DatagramPacket(buffer, buffer.length);
                socket.receive(requestPacket);
                try {
                    DnsRequest dnsRequest = DnsRequest.convert(requestPacket);
                    System.out.println("DNS Request: " + dnsRequest);
                    DnsResponse dnsResponse = dnsResponseGenerator.getDnsResponse(dnsRequest);
                    System.out.println("DNS Response: " + dnsResponse);
                    DatagramPacket responsePacket = createDatagramPacket(requestPacket, dnsResponse);
                    socket.send(responsePacket);
                } catch (DnsException exception) {
                    DnsResponse dnsResponse = new ErrorDnsResponse(exception);
                    DatagramPacket responsePacket = createDatagramPacket(requestPacket, dnsResponse);
                    socket.send(responsePacket);
                }
            }
        } catch (Exception exception) {
            exception.printStackTrace();
        }
    }
}
