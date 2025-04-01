package org.example.client;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.*;
import java.util.Scanner;

public class DnsClient {
    private static final int DNS_PORT = 5354;
    private static String domainName;
    private static String dnsServerAddress = "255.255.255.255";
    private static int clientPort;

    public static void main(String[] args) {
        System.out.println("DNS Client start");
        if (args.length < 3) {
            System.out.println("Usage: java DnsClient <domain> <address> <port>");
            return;
        }
        domainName = args[0];
        String ipAddress = args[1];
        clientPort = Integer.parseInt(args[2]);
        discoverDnsServer();
        registerWithDns(domainName, ipAddress + ":" + clientPort);
        startHttpServer();
        startUserInputListener(); // Новая функция для пользовательского ввода
    }
    private static void discoverDnsServer() {
        try (DatagramSocket socket = new DatagramSocket()) {
            socket.setBroadcast(true);
            String message = "DISCOVER_DNS";
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(
                    buffer,
                    buffer.length,
                    InetAddress.getByName(dnsServerAddress),
                    DNS_PORT
            );
            socket.send(packet);
            byte[] responseBuffer = new byte[256];
            DatagramPacket responsePacket = new DatagramPacket(responseBuffer, responseBuffer.length);
            socket.receive(responsePacket);
            dnsServerAddress = new String(responsePacket.getData(), 0, responsePacket.getLength());
            System.out.println("DNS Server found at: " + dnsServerAddress);
        } catch (Exception e) {
            System.out.println("Could not discover DNS server, using default.");
        }
    }

    private static void registerWithDns(String domain, String addressWithPort) {
        try (DatagramSocket socket = new DatagramSocket()) {
            String message = "REGISTER " + domain + " " + addressWithPort;
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(
                    buffer,
                    buffer.length,
                    InetAddress.getByName(dnsServerAddress),
                    DNS_PORT
            );
            socket.send(packet);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void startHttpServer() {
        new Thread(() -> {
            try (ServerSocket serverSocket = new ServerSocket(clientPort)) {
                System.out.println("HTTP Server running on port " + clientPort);
                while (true) {
                    Socket client = serverSocket.accept();
                    PrintWriter out = new PrintWriter(client.getOutputStream(), true);
                    out.println("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
                    out.println("<html><body><h1>Welcome to " + domainName + "</h1></body></html>");
                    out.close();
                    client.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    private static void startUserInputListener() {
        new Thread(() -> {
            try (Scanner scanner = new Scanner(System.in)) {
                while (true) {
                    System.out.println("Enter domain name to connect (or EXIT to quit):");
                    String input = scanner.nextLine();
                    if (input.equalsIgnoreCase("EXIT")) {
                        System.out.println("Exiting...");
                        break;
                    }
                    queryDomain(input);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private static void queryDomain(String domain) {
        try (DatagramSocket socket = new DatagramSocket()) {
            String message = "QUERY " + domain;
            byte[] buffer = message.getBytes();
            DatagramPacket packet = new DatagramPacket(
                    buffer,
                    buffer.length,
                    InetAddress.getByName(dnsServerAddress),
                    DNS_PORT
            );
            socket.send(packet);

            byte[] responseBuffer = new byte[256];
            DatagramPacket responsePacket = new DatagramPacket(responseBuffer, responseBuffer.length);
            socket.receive(responsePacket);
            String response = new String(responsePacket.getData(), 0, responsePacket.getLength());
            System.out.println("DNS Response for " + domain + ": " + response);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

