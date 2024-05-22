package cz.cvut.fel.agents.pdv.swim;

import cz.cvut.fel.agents.pdv.dsand.Message;

public class SWIMMessage extends Message {
    private final MessageType type;
    private final String waitingForAckProcess;
    private final String notRespondingProcess;

    private SWIMMessage(MessageType type, String awaitingAckProcess, String notRespondingProcess) {
        super();
        this.type = type;
        this.waitingForAckProcess = awaitingAckProcess;
        this.notRespondingProcess = notRespondingProcess;
    }

    public static SWIMMessage createShortDistanceMessage(MessageType type) {
        return new SWIMMessage(type, null, null);
    }

    public static SWIMMessage createLongDistanceMessage(MessageType type, String waitingForAckProcess, String notRespondingProcess) {
        return new SWIMMessage(type, waitingForAckProcess, notRespondingProcess);
    }

    public MessageType getType() {
        return type;
    }

    public String getWaitingForAckProcess() {
        return waitingForAckProcess;
    }

    public String getNotRespondingProcess() {
        return notRespondingProcess;
    }

    public enum MessageType {
        POKE,
        ACK,
        FURTHER_ACK,
        FURTHER_POKE,
        REPOST_ACK,
        REPOST_POKE
    }
}


