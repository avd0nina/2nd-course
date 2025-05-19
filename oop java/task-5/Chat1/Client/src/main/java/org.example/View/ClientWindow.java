package org.example.View;

import org.example.Message.Message;
import org.example.View.ClientWindowListener;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.List;

public class ClientWindow extends JFrame {
    private UserListPanel userListPanel;
    private JPanel messagePanel;
    private JTextField inputField;
    private JButton sendButton;
    private ClientWindowListener listener;

    public ClientWindow() {
        setTitle("Chat Client");
        setSize(700, 500);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        setLocationRelativeTo(null);

        messagePanel = new JPanel();
        messagePanel.setLayout(new BoxLayout(messagePanel, BoxLayout.Y_AXIS));
        JScrollPane scrollPane = new JScrollPane(messagePanel);
        scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

        inputField = new JTextField();
        sendButton = new JButton("Send");

        JPanel inputPanel = new JPanel(new BorderLayout());
        inputPanel.add(inputField, BorderLayout.CENTER);
        inputPanel.add(sendButton, BorderLayout.EAST);

        userListPanel = new UserListPanel();
        userListPanel.setPreferredSize(new Dimension(200, 0));

        JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, scrollPane, userListPanel);
        splitPane.setDividerLocation(500);

        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(splitPane, BorderLayout.CENTER);
        getContentPane().add(inputPanel, BorderLayout.SOUTH);

        setVisible(true);

        sendButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                handleSend();
            }
        });

        inputField.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                handleSend();
            }
        });

        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                if (listener != null) {
                    listener.stopWorking();
                }
                System.exit(0);
            }
        });
    }

    private void handleSend() {
        String text = inputField.getText();
        if (!text.isEmpty() && listener != null) {
            listener.sendMessage(text);
            inputField.setText("");
        }
    }

    public void addMessage(Message message) {
        String username = message.getUsername();
        String text = message.getText();
        String time = message.getSendingTime();
        MessagePanel messagePanelComponent = new MessagePanel(username, text, time);
        messagePanelComponent.setAlignmentX(Component.LEFT_ALIGNMENT);
        messagePanel.add(messagePanelComponent);
        messagePanel.revalidate();
        messagePanel.repaint();
        JScrollBar vertical = ((JScrollPane) messagePanel.getParent().getParent()).getVerticalScrollBar();
        vertical.setValue(vertical.getMaximum());
    }

    public void updateUserList(List<String> users) {
        userListPanel.updateUsers(users);
    }

    public void setListener(ClientWindowListener listener) {
        this.listener = listener;
    }

    public void setCurrentUsername(String username) {
        userListPanel.setCurrentUsername(username);
    }
}
