package cz.cvut.fel.agents.pdv.failure_detector;

import cz.cvut.fel.agents.pdv.dsand.Message;

public class HeartbeatMessage extends Message {
    public MessageType msgCode;

    public HeartbeatMessage(MessageType msgCode) {
        super();
        this.msgCode = msgCode;
    }

    public enum MessageType {
        HEARTBEAT_CHECK,
        OK
    }
}
