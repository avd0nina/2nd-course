package org.example;

import java.io.IOException;
import java.net.*;
import java.util.*;

public class DHCPServer {
    private static final int PORT = 6767;
    private static final String NETWORK_PREFIX = "192.168.0.";
    private volatile boolean isRunning = true;
    private int nextIp = 1;
    private Set<String> leasedIps = Collections.synchronizedSet(new HashSet<>());

    public void start() {
        new Thread(this::runServer).start();
    }

    private void runServer() {
        try (DatagramSocket socket = new DatagramSocket(PORT)) {
            socket.setBroadcast(true);
            System.out.println("DHCP Server started on port " + PORT);
            while (isRunning) {
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);

                String message = new String(packet.getData()).trim();
                InetAddress clientAddress = packet.getAddress();
                int clientPort = packet.getPort();

                if (message.equals("DHCPDISCOVER")) {
                    String ip = getNextAvailableIp();
                    String response = "DHCPOFFER " + ip;
                    sendResponse(socket, response, clientAddress, clientPort);
                }
            }
        } catch (IOException e) {
            if (isRunning) {
                System.err.println("DHCP Server error: " + e.getMessage());
            }
        }
    }

    private String getNextAvailableIp() {
        String ip;
        do {
            ip = NETWORK_PREFIX + nextIp;
            nextIp = (nextIp % 254) + 1; // 192.168.0.1 - 192.168.0.254
        } while (leasedIps.contains(ip));
        leasedIps.add(ip);
        return ip;
    }

    private void sendResponse(DatagramSocket socket, String message,
                              InetAddress address, int port) throws IOException {
        byte[] data = message.getBytes();
        DatagramPacket packet = new DatagramPacket(data, data.length, address, port);
        socket.send(packet);
    }

    public void stop() {
        isRunning = false;
    }

    public static void main(String[] args) {
        DHCPServer server = new DHCPServer();
        server.start();
        try {
            Thread.sleep(Long.MAX_VALUE);
        } catch (InterruptedException e) {
            server.stop();
        }
    }
}
