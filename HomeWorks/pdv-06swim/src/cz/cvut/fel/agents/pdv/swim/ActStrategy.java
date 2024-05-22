package cz.cvut.fel.agents.pdv.swim;

import cz.cvut.fel.agents.pdv.dsand.Message;
import cz.cvut.fel.agents.pdv.dsand.Pair;

import java.util.*;
import java.util.Map.Entry;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * Trida s implementaci metody act() pro proces Failure Detector. Tuto tridu (a tridy pouzivanych zprav) budete
 * odevzdavat. Do tridy si muzete doplnit vlastni pomocne datove struktury. Hodnoty muzete inicializovat primo
 * v konstruktoru. Klicova je metoda act(), kterou vola kazda instance tridy FailureDetectorProcess ve sve metode
 * act(). Tuto metodu naimplementujte podle protokolu SWIM predstaveneho na prednasce.
 *
 * Pokud si stale jeste nevite rady s frameworkem, inspiraci muzete nalezt v resenych prikladech ze cviceni.
 */
public class ActStrategy {
    private static int ProcessIndexCounter = 0;
    private final int processIndex;

    // maximalni zpozdeni zprav
    private final int maxDelayForMessages;
    private final List<String> otherProcesses;

    private Map<String, Integer> awaitingAck = new HashMap<>();
    private Map<String, Integer> awaitingFurtherAck = new HashMap<>();
    private final Random random = new Random();

    private int timeToDetectKilledProcess;
    private int upperBoundOnMessages;
    private int tact = 0;
    private int pokeInterval = 10;
    private int messagesSent = 0;

    private int offset;

    // Definujte vsechny sve promenne a datove struktury, ktere budete potrebovat

    public ActStrategy(int maxDelayForMessages, List<String> otherProcesses,
                       int timeToDetectKilledProcess, int upperBoundOnMessages) {
        this.maxDelayForMessages = maxDelayForMessages;
        this.otherProcesses = otherProcesses;

        this.timeToDetectKilledProcess = timeToDetectKilledProcess;
        this.upperBoundOnMessages = upperBoundOnMessages;
        this.offset = random.nextInt(otherProcesses.size());
        this.processIndex = ProcessIndexCounter++;
    }

    /**
     * Metoda je volana s kazdym zavolanim metody act v FailureDetectorProcess. Metodu implementujte
     * tak, jako byste implementovali metodu act() v FailureDetectorProcess, misto pouzivani send()
     * pridejte zpravy v podobe paru - prijemce, zprava do listu. Zpravy budou nasledne odeslany.
     * <p>
     * Diky zavedeni teto metody muzeme kontrolovat pocet odeslanych zprav vasi implementaci.
     */
    public List<Pair<String, Message>> act(Queue<Message> inbox, String disseminationProcess) {
        List<Pair<String, Message>> messagesToSend = new ArrayList<>();

//        if (tact % pokeInterval == 0) {
//            if (random.nextDouble() < 0.1) {
//                String nextToPoke = otherProcesses.stream()
//                        .filter(p -> !awaitingAck.containsKey(p) && !awaitingFurtherAck.containsKey(p))
//                        .findAny()
//                        .orElse(null);
//                pokeProcess(nextToPoke, messagesToSend);
//            }
//        }

        if (awaitingAck.isEmpty() && awaitingFurtherAck.isEmpty()) {
            String nextToPoke = otherProcesses.get(random.nextInt(otherProcesses.size()));

            pokeProcess(nextToPoke, messagesToSend);
        }

//        String nextToPoke = otherProcesses.get((tact + offset) % otherProcesses.size());
//        pokeProcess(nextToPoke, messagesToSend);

        respondToMessages(inbox, messagesToSend);

//      Timeouts
//        1. increment all awaiting acknowledgement timers
        awaitingAck = awaitingAck.entrySet().stream()
                .collect(Collectors.toMap(Entry::getKey, entry -> entry.getValue() + 1));

//        2. check for timeouts
        Set<String> timedOutProcesses = awaitingAck.entrySet().stream()
                .filter(entry -> entry.getValue() > maxDelayForMessages)
                .map(Entry::getKey)
                .collect(Collectors.toSet());

//        3. increment all awaiting further acknowledgement timers
        awaitingFurtherAck = awaitingFurtherAck.entrySet().stream()
                .collect(Collectors.toMap(Entry::getKey, entry -> entry.getValue() + 1));

//        4. for each timed out process,
//          send further poke message,
//          through 3 other processes that are not in timeout
        timedOutProcesses.forEach(timedOutProcess -> {
            List<String> availableProcesses = this.otherProcesses.stream()
                    .filter(p -> !timedOutProcesses.contains(p) && !p.equals(timedOutProcess))
                    .filter(p -> !awaitingFurtherAck.containsKey(p) && !awaitingAck.containsKey(p))
                    .toList();

            IntStream.range(0, 10)
                    .mapToObj(i -> availableProcesses.get(random.nextInt(availableProcesses.size())))
                    .forEach(p -> {
                        messagesToSend.add(new Pair<>(p, SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.FURTHER_POKE, null, timedOutProcess)));
                        messagesSent++;
                    });

            awaitingFurtherAck.put(timedOutProcess, 0);
            awaitingAck.remove(timedOutProcess);
            pokeInterval--;
        });



//        5. check for further timeouts
        Set<String> timedOutFurtherProcesses = awaitingFurtherAck.entrySet().stream()
                .filter(entry -> entry.getValue() > maxDelayForMessages * 4)
                .map(Entry::getKey)
                .collect(Collectors.toSet());

//        timedOutFurtherProcesses
//                .forEach(timedOutFurtherProcess -> {
//                    System.err.println("Process " + timedOutFurtherProcess + " failed");
//                });

//        6. for each timed out further process,
//          check with Dissemination if truly failed
        timedOutFurtherProcesses.forEach(process -> {
            messagesToSend.add(new Pair<>(disseminationProcess, new PFDMessage(process)));
//            System.err.println(this.processIndex + ":Checking process " + process + " with Dissemination");
            awaitingFurtherAck.remove(process);
            messagesSent++;
        });

        tact++;

        return messagesToSend;
    }

    private void pokeProcess(String process, List<Pair<String, Message>> messagesToSend){
        messagesToSend.add(new Pair<>(process, SWIMMessage.createShortDistanceMessage(SWIMMessage.MessageType.POKE)));
        awaitingAck.put(process, 0);
        messagesSent++;
    }

    private void respondToMessages(Queue<Message> inbox, List<Pair<String, Message>> messagesToSend){
        while (!inbox.isEmpty()) {
            Message message = inbox.poll();
            respondToMsg(message, messagesToSend);
        }
    }

    private void respondToMsg(Message message, List<Pair<String, Message>> messagesToSend) {
        if (message instanceof SWIMMessage msg) {
            switch (msg.getType()){
                case ACK -> {
                    awaitingAck.remove(msg.sender);
                    awaitingFurtherAck.remove(msg.sender);
                    pokeInterval++;
                }
                case POKE -> {
                    messagesToSend.add(new Pair<>(msg.sender, SWIMMessage.createShortDistanceMessage(SWIMMessage.MessageType.ACK)));
                    messagesSent++;
                }
                case FURTHER_POKE -> {
                    messagesToSend.add(new Pair<>(msg.getNotRespondingProcess(), SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.REPOST_POKE, msg.sender, msg.getNotRespondingProcess())));
                    messagesSent++;
                }
                case REPOST_POKE -> {
                    messagesToSend.add(new Pair<>(msg.sender, SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.FURTHER_ACK, msg.getWaitingForAckProcess(), msg.getNotRespondingProcess())));
                    messagesSent++;
                }
                case FURTHER_ACK -> {
                    messagesToSend.add(new Pair<>(msg.getWaitingForAckProcess(), SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.REPOST_ACK, msg.getWaitingForAckProcess(), msg.getNotRespondingProcess())));
                    messagesSent++;
                }
                case REPOST_ACK -> {
                    awaitingFurtherAck.remove(msg.getNotRespondingProcess());
                    awaitingAck.remove(msg.getNotRespondingProcess());
                    pokeInterval++;
                }
            }
        }
//        else if (message instanceof DeadProcessMessage deadProcessMessage) {
//            System.err.println("Spravne detekovan ukonceny proces " + deadProcessMessage.getProcessID());
//        }
    }


}
