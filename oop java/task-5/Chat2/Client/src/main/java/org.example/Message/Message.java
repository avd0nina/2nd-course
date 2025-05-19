package org.example.Message;

import java.io.Serializable;

public class Message implements Serializable {
    private String username;
    private String text;
    private String sendingTime;

    public Message(String username, String text, String sendingTime) {
        this.username = username;
        this.sendingTime = sendingTime;
        this.text = text;
    }

    public String getUsername() {
        return username;
    }

    public String getText() {
        return text;
    }

    public String getSendingTime() {
        return sendingTime;
    }
}
