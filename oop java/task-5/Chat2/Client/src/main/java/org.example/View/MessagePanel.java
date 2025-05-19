package org.example.View;

import javax.swing.*;
import java.awt.*;

public class MessagePanel extends JPanel {
    public MessagePanel(String username, String text, String time) {
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        JLabel userLabel = new JLabel(time + " " + username);
        userLabel.setFont(new Font("Arial", Font.BOLD, 12));
        JLabel messageLabel = new JLabel(text);
        messageLabel.setFont(new Font("Arial", Font.PLAIN, 14));
        add(userLabel);
        add(Box.createRigidArea(new Dimension(0, 5)));
        add(messageLabel);
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
    }
}
