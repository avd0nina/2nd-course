package org.example;

import org.example.Server.Server;

import java.io.IOException;
import java.util.Objects;

public class Main {
    public static void main(String[] args) throws IOException {
        Server server = new Server();
        server.onServer();
    }
}
