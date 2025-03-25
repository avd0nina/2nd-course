package org.example.client;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.*;
import java.util.Scanner;

public class DnsClient {
    private static final int DNS_PORT = 5354;
    private static final int HTTP_PORT = 8080;
    private static String domainName;
    private static String dnsServerAddress = "255.255.255.255";

    public static void main(String[] args) {
        System.out.println("DNS Client start");
        if (args.length < 2) {
            System.out.println("Usage: java DnsClient <domain> <address>");
            return;
        }
        domainName = args[0];
        String ipAddress = args[1];
        discoverDnsServer();
        registerWithDns(domainName, ipAddress);
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

    private static void registerWithDns(String domain, String ip) {
        try (DatagramSocket socket = new DatagramSocket()) {
            String message = "REGISTER " + domain + " " + ip;
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
            try (ServerSocket serverSocket = new ServerSocket(HTTP_PORT)) {
                System.out.println("HTTP Server running on port " + HTTP_PORT);
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
                    System.out.println("Enter a command (SEND <domain> <message> or EXIT):");
                    String input = scanner.nextLine();
                    if (input.equalsIgnoreCase("EXIT")) {
                        System.out.println("Exiting...");
                        break;
                    }
                    String[] parts = input.split(" ", 3);
                    if (parts.length < 3 || !parts[0].equalsIgnoreCase("SEND")) {
                        System.out.println("Invalid command. Usage: SEND <domain> <message>");
                        continue;
                    }
                    String domainName = parts[1];
                    String message = parts[2];
                    sendMessageToClient(domainName, message);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private static void sendMessageToClient(String domainName, String message) {
        try (DatagramSocket socket = new DatagramSocket()) {
            String queryMessage = "QUERY " + domainName;
            byte[] queryBuffer = queryMessage.getBytes();
            DatagramPacket queryPacket = new DatagramPacket(
                    queryBuffer,
                    queryBuffer.length,
                    InetAddress.getByName(dnsServerAddress),
                    DNS_PORT
            );
            socket.send(queryPacket);
            byte[] responseBuffer = new byte[256];
            DatagramPacket responsePacket = new DatagramPacket(responseBuffer, responseBuffer.length);
            socket.receive(responsePacket);
            String clientIp = new String(responsePacket.getData(), 0, responsePacket.getLength());
            if (clientIp.startsWith("ERROR")) {
                System.out.println("Error: " + clientIp);
                return;
            }
            System.out.println("Client IP found: " + clientIp);
            byte[] messageBuffer = message.getBytes();
            DatagramPacket messagePacket = new DatagramPacket(
                    messageBuffer,
                    messageBuffer.length,
                    InetAddress.getByName(clientIp),
                    HTTP_PORT
            );
            socket.send(messagePacket);
            System.out.println("Message sent to client: " + domainName);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
