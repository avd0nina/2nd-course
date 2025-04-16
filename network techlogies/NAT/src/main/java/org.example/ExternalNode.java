package org.example;

import java.io.*;
import java.net.*;

public class ExternalNode {
    private ServerSocket serverSocket;
    private boolean isRunning = false;

    public void start(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        isRunning = true;
        System.out.println("External node started on port " + port);

        while (isRunning) {
            try (Socket clientSocket = serverSocket.accept();
                 BufferedReader in = new BufferedReader(
                         new InputStreamReader(clientSocket.getInputStream()));
                 PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true)) {

                String request = in.readLine();
                System.out.println("External node received: " + request);

                if (request.startsWith("PING")) {
                    out.println("PONG from external node");
                }
            } catch (IOException e) {
                if (isRunning) System.err.println("External node error: " + e.getMessage());
            }
        }
    }
}
