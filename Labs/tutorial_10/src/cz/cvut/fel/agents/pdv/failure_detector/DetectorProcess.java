package cz.cvut.fel.agents.pdv.failure_detector;

import cz.cvut.fel.agents.pdv.dsand.DSProcess;
import cz.cvut.fel.agents.pdv.dsand.Message;

import java.util.*;
import java.util.function.BiConsumer;

/**
 * Trida DetectorProcess. Ukolem kazde instance je detekovat ukoncene procesy pomoci all-to-all heartbeat algoritmu.
 * Krome implementace samotne metody act() budete pravdepodobne potrebovat zavest pomocne datove struktury.
 *
 * V metode act() mate napovedu k postupu.
 * Pokud si stale jeste nevite rady s frameworkem, inspiraci muzete nalezt v resenych prikladech ze cviceni.
 */
public class DetectorProcess extends DSProcess {

    // parametry posilani zprav
    // jak casto se maji zpravy posilat
    private final int period;
    // limit k prijeti heartbeatu
    private final int timeout;

    // interni pocitadlo, kolikrat byl proces vzbuzen
    // procesy se budi v kazdem kroku simulace vsechny
    // jejich wakeCounters tedy nemaji zadny drift
    private int wakeCount = 0;


    // TODO
    // doplnte vlastni datove struktury
    private Map<String, Integer> stillAliveProcesses;

    // vsechny procesy
    private final List<String> otherProcesses;

    public DetectorProcess(String id, Queue<Message> inbox, BiConsumer<String, Message> outbox,
                           int maxDelay, List<String> otherProcesses) {
        super(id, inbox, outbox);

        this.period = 10;
        this.otherProcesses = otherProcesses;

        // TODO
        // Nastavte timeout
        this.timeout = 12;

        // TODO
        // inicializujte vlastni datove struktury
        stillAliveProcesses = new HashMap<>();
        for (String processId : otherProcesses) {
            stillAliveProcesses.put(processId, 0);
        }
    }

    @Override
    public void act() {
        // Muzete predpokladat, ze se zpravy neztraceji, jen zpozduji...
        // 1. zajistete, ze se s kazdou periodou poslou heartbeats vsem ostatnim procesum
        if (wakeCount % period == 0) {
            for (String processId : otherProcesses) {
                if (stillAliveProcesses.get(processId) <= timeout) {
                    send(processId, new HeartbeatMessage(HeartbeatMessage.MessageType.HEARTBEAT_CHECK));
                }
            }
        }

        // 2. pokud obdrzite heartbeat od jineho procesu, aktualizujte si databazi zivych procesu
        while (!inbox.isEmpty()) {
            Message message = inbox.poll();
            if (message instanceof HeartbeatMessage heartbeatMessage) {
                if (heartbeatMessage.msgCode == HeartbeatMessage.MessageType.HEARTBEAT_CHECK) {
                    send(heartbeatMessage.sender, new HeartbeatMessage(HeartbeatMessage.MessageType.OK));
                }
                if (heartbeatMessage.msgCode == HeartbeatMessage.MessageType.OK) {
                    stillAliveProcesses.put(heartbeatMessage.sender, 0);
                }
            }
        }


        // 3. detekujte havarovane procesy, takovy proces vypiste prave jednou
        for (String processId : stillAliveProcesses.keySet()) {
            if ((stillAliveProcesses.get(processId) > timeout) ){
                System.out.println("Process " + processId + " has failed.");
                stillAliveProcesses.put(processId, Integer.MIN_VALUE);
            } else if (stillAliveProcesses.get(processId) != Integer.MIN_VALUE) {
                stillAliveProcesses.put(processId, stillAliveProcesses.get(processId) + 1);
            }
        }

        // inkrementujeme citac probuzeni procesu
        ++wakeCount;

        if (wakeCount == timeout * 4) {
            terminateAll();
        }
    }
}
