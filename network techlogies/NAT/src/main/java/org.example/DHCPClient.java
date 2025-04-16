package org.example;

import java.io.IOException;
import java.net.*;

public class DHCPClient {
    private static final int DHCP_SERVER_PORT = 6767;
    private static final int TIMEOUT_MS = 5000;

    public static String discoverIp() throws IOException {
        try (DatagramSocket socket = new DatagramSocket()) {
            socket.setSoTimeout(TIMEOUT_MS);

            String discoverMsg = "DHCPDISCOVER";
            byte[] sendData = discoverMsg.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(
                    sendData, sendData.length,
                    InetAddress.getByName("255.255.255.255"), DHCP_SERVER_PORT);
            socket.send(sendPacket);

            byte[] receiveData = new byte[1024];
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            socket.receive(receivePacket);

            String response = new String(receivePacket.getData()).trim();
            String[] parts = response.split(" ");
            if (parts.length >= 2) {
                return parts[1]; // возвращаем ip
            }
        }
        throw new IOException("DHCP discovery failed");
    }
}
