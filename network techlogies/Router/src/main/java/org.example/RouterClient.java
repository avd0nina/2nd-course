package org.example;

import java.io.*;
import java.net.Socket;

public class RouterClient {
    private final String ip;
    private final String mac;
    private final Socket socket;
    private final PrintWriter out;
    private final BufferedReader in;

    public RouterClient(String ip, String mac, Socket socket) throws IOException {
        this.ip = ip;
        this.mac = mac;
        this.socket = socket;
        this.out = new PrintWriter(socket.getOutputStream(), true);
        this.in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    public String getIp() { return ip; }
    public String getMac() { return mac; }

    public void sendMessage(String message) {
        out.println(message);
    }

    public String readMessage() throws IOException {
        return in.readLine();
    }

    public void close() {
        try {
            in.close();
            out.close();
            socket.close();
        } catch (IOException e) {
            System.err.println("Error closing client connection: " + e.getMessage());
        }
    }
}
