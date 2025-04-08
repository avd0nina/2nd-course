package org.example;

import java.io.*;
import java.net.*;
import java.util.*;

public class Client {
    private String ip;
    private final String mac;
    private Socket routerSocket;
    private PrintWriter out;
    private BufferedReader in;
    private boolean connected = false;

    public Client() {
        this.ip = null; // теперь ip будет назначаться DHCP
        this.mac = generateMac();
    }

    private String generateMac() {
        Random random = new Random();
        return String.format("%02X-%02X-%02X-%02X-%02X-%02X",
                random.nextInt(256), random.nextInt(256),
                random.nextInt(256), random.nextInt(256),
                random.nextInt(256), random.nextInt(256));
    }

    public boolean isConnected() {
        return connected;
    }

    public void disconnect() throws IOException {
        if (connected) {
            out.println("DISCONNECT");
            connected = false;
            routerSocket.close();
        }
    }

    public void connectToRouter(String routerIp, int routerPort) throws IOException {
        this.ip = DHCPClient.discoverIp(); // получаем ip от DHCP
        System.out.println("Got IP from DHCP: " + ip);

        routerSocket = new Socket(routerIp, routerPort);
        out = new PrintWriter(routerSocket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(routerSocket.getInputStream()));
        out.println("CONNECT " + ip + " " + mac);
        String response = in.readLine();
        if (response != null && response.startsWith("CONNECTED")) {
            connected = true;
            new Thread(this::listenForResponses).start();
            System.out.println("Connected to router. IP: " + ip + ", MAC: " + mac);
        } else {
            throw new IOException("Connection failed: " + response);
        }
    }

    private void listenForResponses() {
        try {
            String response;
            while ((response = in.readLine()) != null) {
                System.out.println("Response: " + response);
            }
        } catch (IOException e) {
            if (connected) {
                System.err.println("Connection lost: " + e.getMessage());
            }
        } finally {
            connected = false;
        }
    }

    public void sendPing(String targetIp) throws IOException {
        if (!connected) throw new IOException("Not connected to router");
        out.println("PING " + targetIp);
    }

    public static void main(String[] args) {
        Client client = new Client();
        Scanner scanner = new Scanner(System.in);
        try {
            System.out.print("Enter router IP (default localhost): ");
            String routerIp = scanner.nextLine().trim();
            if (routerIp.isEmpty()) routerIp = "localhost";
            System.out.print("Enter router port (default 8080): ");
            String portStr = scanner.nextLine().trim();
            int routerPort = portStr.isEmpty() ? 8080 : Integer.parseInt(portStr);
            client.connectToRouter(routerIp, routerPort);
            while (client.isConnected()) {
                System.out.print("Enter command (PING <ip>, DISCONNECT): ");
                String command = scanner.nextLine().trim();
                if (command.startsWith("PING")) {
                    client.sendPing(command.split(" ")[1]);
                } else if (command.equals("DISCONNECT")) {
                    client.disconnect();
                } else {
                    System.out.println("Unknown command");
                }
            }
        } catch (Exception e) {
            System.err.println("Error: " + e.getMessage());
        } finally {
            scanner.close();
        }
    }
}
