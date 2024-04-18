package cz.cvut.fel.agents.pdv.bfs;

import cz.cvut.fel.agents.pdv.demo.DummyMessage;
import cz.cvut.fel.agents.pdv.dsand.DSProcess;
import cz.cvut.fel.agents.pdv.dsand.Message;
import cz.cvut.fel.agents.pdv.dsand.MessageWrapper;

import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.function.BiConsumer;

public class BFSProcess extends DSProcess {

    /**
     * Seznam nasledniku uzlu spravovaneho aktualnim procesem
     */
    private List<String> neighbors;

    /**
     * Je uzel spravovany aktualnim procesem 'root' prohledavani?
     */
    private boolean isRoot;

    /**
     * Je uzel spravovany aktualnim procesem cilovym uzlem?
     */
    private boolean isGoal;

    /**
     * Identifikator predchazejiciho procesu ('uzlu' na nejkratsi ceste)
     */
    private String predecessor = null;
    private boolean visited = false;
    private boolean sentBackDropMessage = false;

    public BFSProcess(String id, Queue<Message> inbox, BiConsumer<String, Message> outbox,
                      List<String> neighbors, boolean isRoot, boolean isGoal) {
        super(id, inbox, outbox);
        this.neighbors = neighbors;
        this.isRoot = isRoot;
        this.isGoal = isGoal;

        // Pro jednoduchost nastavime predchazeji proces korene na koren samotny.
        if(isRoot) predecessor = id;
    }

    @Override
    public void act() {
        if (isRoot && !visited) {
            ArrayList<String> path = new ArrayList<>();
            for (String neighbor : neighbors) {
                send(neighbor, new BFSMessage(path));
            }
            visited = true;
        }

        while (!inbox.isEmpty()) {
//            Message msg = inbox.poll();
//            predecessor = msg.sender;
//            System.out.println("Got message from Predecessor: " + predecessor);
//            ArrayList<String> path = new ArrayList<>(((BFSMessage) msg).getPath());
//            path.add(predecessor);
//
//            if ( !visited ){
//                visited = true;
//                if (isGoal) {
//                    System.out.println("Path found!");
//                    for (String node : path) {
//                        System.out.printf("%s -> ", node);
//                    }
//                    System.out.printf("%s\n", id);
//                    terminateAll();
//                } else {
//                    System.out.println("Forwarding message");
//                    for (String neighbor : neighbors) {
//                        send(neighbor, new BFSMessage(path));
//                    }
//                }
//            }
            Message msg = inbox.poll();

            if ((msg instanceof BackDropMessage) && !sentBackDropMessage) {
                if (isRoot) {
                    System.out.println("Path found and returned to root!");
                    System.out.println(((BackDropMessage) msg).getMessage());
                    terminateAll();
                } else {
                    send(predecessor, new BackDropMessage(((BackDropMessage) msg).getMessage()));
                    sentBackDropMessage = true;
                }
            }

            if ( !visited ){
                ArrayList<String> path = new ArrayList<>(((BFSMessage) msg).getPath());
                predecessor = msg.sender;
                path.add(predecessor);
                visited = true;
                System.out.println("Got message from Predecessor: " + predecessor);

                if (isGoal) {
                    System.out.println("Path found!");
                    StringBuilder pathString = new StringBuilder();
                    for (String node : path) {
                        pathString.append(node).append(" -> ");
                    }
                    pathString.append(id).append("\n");
                    send(predecessor, new BackDropMessage(pathString.toString()));
                } else {
                    System.out.println("Forwarding message");
                    for (String neighbor : neighbors) {
                        send(neighbor, new BFSMessage(path));
                    }
                }
            }
        }

        /*
            Doplnte algoritmus, ktery bude vykonavat kazdy proces

            1) Zpracujte prichozi zpravy v 'inbox'
            2) Vytvorte tridy pro posilane zpravy
            3) Doplnte logiku reakce na zpravu a odeslani novych zprav
         */
    }

    @Override
    public boolean isTerminated() {
        return false;
    }
}
