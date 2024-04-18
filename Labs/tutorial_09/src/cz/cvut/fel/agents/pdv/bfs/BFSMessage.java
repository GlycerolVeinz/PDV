package cz.cvut.fel.agents.pdv.bfs;

import cz.cvut.fel.agents.pdv.dsand.Message;

import java.util.ArrayList;
import java.util.List;

public class BFSMessage extends Message {
    private final ArrayList<String> path;

    public BFSMessage(List<String> path) {
        super();
        this.path = new ArrayList<>(path);
    }

    public ArrayList<String> getPath() {
        return new ArrayList<>(path);
    }
}
