package org.example.View;

import org.example.Message.Message;

public interface ClientWindowListener {
    void sendMessage(String message);
    void stopWorking();
}
