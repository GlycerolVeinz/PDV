package cz.cvut.fel.agents.pdv.bfs;

import cz.cvut.fel.agents.pdv.dsand.Message;

public class BackDropMessage extends Message {
    private final String message;
    public BackDropMessage(String message) {
        super();
        this.message = message;
    }

    public String getMessage() {
        return message;
    }
}
