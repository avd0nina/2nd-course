package org.example;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;

public class Router {
    private final Map<String, RouterClient> ipToClient = new ConcurrentHashMap<>();
    private final Map<String, RouterClient> macToClient = new ConcurrentHashMap<>();
    private final Map<Integer, PortMapping> natTable = new ConcurrentHashMap<>();
    private final String publicIp = "203.0.113.1";
    private ServerSocket serverSocket;
    private ExecutorService executor = Executors.newCachedThreadPool();
    private boolean isRunning = false;
    private int nextNatPort = 10000;

    private static class PortMapping {
        final String privateIp;
        final int privatePort;
        final long expirationTime;

        PortMapping(String privateIp, int privatePort) {
            this.privateIp = privateIp;
            this.privatePort = privatePort;
            this.expirationTime = System.currentTimeMillis() + 300000; // 5 минут
        }
    }

    private boolean isLocalIp(String ip) {
        return ip.startsWith("192.168.0.") || ip.startsWith("10.0.0.");
    }

    private void handlePing(RouterClient sender, String targetIp) {
        if (!isLocalIp(targetIp)) {
            handleExternalPing(sender, targetIp);
            return;
        }

        RouterClient target = ipToClient.get(targetIp);
        if (target == null) {
            sender.sendMessage("PING ERROR: Host " + targetIp + " not found");
        } else {
            try {
                target.sendMessage("PING CHECK");
                sender.sendMessage("PING REPLY from " + targetIp);
            } catch (Exception e) {
                sender.sendMessage("PING ERROR: Host " + targetIp + " is offline");
            }
        }
    }

    private void handleExternalPing(RouterClient sender, String externalIp) {
        try {
            int clientPort = sender.getSocket().getPort(); // получаем порт клиента из его сокета
            int natPort = assignNatPort(sender.getIp(), clientPort);
            String natIpPort = publicIp + ":" + natPort;

            System.out.println("NAT: Translating " + sender.getIp() + ":" + clientPort + " -> " + natIpPort);
            Thread.sleep(100);

            sender.sendMessage("PING REPLY from " + externalIp + " (via NAT)");
        } catch (Exception e) {
            sender.sendMessage("PING ERROR: Failed to reach " + externalIp);
        }
    }

    private synchronized int assignNatPort(String privateIp, int privatePort) {
        int port = nextNatPort++;
        natTable.put(port, new PortMapping(privateIp, privatePort));
        return port;
    }

    private void handleDisconnect(RouterClient client) {
        if (client.getIp() != null && !client.getIp().isEmpty()) {
            ipToClient.remove(client.getIp());
            macToClient.remove(client.getMac());
            System.out.println("Client disconnected: IP=" + client.getIp());
        }
        client.close();
    }

    private void handleClientCommunication(RouterClient client) {
        try {
            String message;
            while ((message = client.readMessage()) != null) {
                String[] parts = message.split(" ");
                String command = parts[0];
                switch (command) {
                    case "PING":
                        handlePing(client, parts[1]);
                        break;
                    case "DISCONNECT":
                        handleDisconnect(client);
                        return;
                    default:
                        client.sendMessage("ERROR Unknown command");
                }
            }
        } catch (IOException e) {
            System.err.println("Client communication error: " + e.getMessage());
        } finally {
            handleDisconnect(client);
        }
    }

    private void handleNewConnection(Socket clientSocket) {
        try {
            RouterClient client = new RouterClient("", "", clientSocket);
            String message = client.readMessage();
            if (message == null || !message.startsWith("CONNECT")) {
                client.sendMessage("ERROR First message must be CONNECT");
                client.close();
                return;
            }
            String[] parts = message.split(" ");
            if (parts.length < 3) {
                client.sendMessage("ERROR Invalid CONNECT format");
                client.close();
                return;
            }
            String ip = parts[1];
            String mac = parts[2];
            if (ipToClient.containsKey(ip)) {
                client.sendMessage("ERROR IP address " + ip + " already in use");
                client.close();
                return;
            }
            if (macToClient.containsKey(mac)) {
                client.sendMessage("ERROR MAC address " + mac + " already in use");
                client.close();
                return;
            }
            client = new RouterClient(ip, mac, clientSocket);
            ipToClient.put(ip, client);
            macToClient.put(mac, client);
            client.sendMessage("CONNECTED " + ip);
            System.out.println("Client connected: IP=" + ip + ", MAC=" + mac);
            handleClientCommunication(client);
        } catch (IOException e) {
            System.err.println("Connection handling error: " + e.getMessage());
        }
    }

    public void start(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        isRunning = true;
        System.out.println("Router started on port " + port);
        executor.execute(() -> {
            while (isRunning) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    executor.execute(() -> handleNewConnection(clientSocket));
                } catch (IOException e) {
                    if (isRunning) System.err.println("Error accepting client: " + e.getMessage());
                }
            }
        });
    }

    public void stop() {
        isRunning = false;
        executor.shutdownNow();
        try {
            serverSocket.close();
        } catch (IOException e) {
            System.err.println("Error closing server socket: " + e.getMessage());
        }
        ipToClient.values().forEach(RouterClient::close);
        ipToClient.clear();
        macToClient.clear();
    }

    public static void main(String[] args) {
        Router router = new Router();
        try {
            router.start(8080);
            new Thread(() -> {
                try {
                    new ExternalNode().start(9090);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }).start();
            Thread.sleep(Long.MAX_VALUE);
        } catch (Exception e) {
            System.err.println("Router error: " + e.getMessage());
        } finally {
            router.stop();
        }
    }
}
