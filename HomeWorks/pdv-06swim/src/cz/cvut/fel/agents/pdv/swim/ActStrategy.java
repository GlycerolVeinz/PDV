package cz.cvut.fel.agents.pdv.swim;

import cz.cvut.fel.agents.pdv.dsand.Message;
import cz.cvut.fel.agents.pdv.dsand.Pair;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Queue;
import java.util.Random;
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

    // maximalni zpozdeni zprav
    private final int maxDelayForMessages;
    private final List<String> otherProcesses;

    private Map<String, Integer> awaitingAck = new HashMap<>();
    private Map<String, Integer> awaitingFurtherAck = new HashMap<>();
    private final Random random = new Random();

    private int timeToDetectKilledProcess;
    private int upperBoundOnMessages;
    private int messagesSent = 0;

    // Definujte vsechny sve promenne a datove struktury, ktere budete potrebovat

    public ActStrategy(int maxDelayForMessages, List<String> otherProcesses,
                       int timeToDetectKilledProcess, int upperBoundOnMessages) {
        this.maxDelayForMessages = maxDelayForMessages;
        this.otherProcesses = otherProcesses;

        this.timeToDetectKilledProcess = timeToDetectKilledProcess;
        this.upperBoundOnMessages = upperBoundOnMessages;
    }

    /**
     * Metoda je volana s kazdym zavolanim metody act v FailureDetectorProcess. Metodu implementujte
     * tak, jako byste implementovali metodu act() v FailureDetectorProcess, misto pouzivani send()
     * pridejte zpravy v podobe paru - prijemce, zprava do listu. Zpravy budou nasledne odeslany.
     * <p>
     * Diky zavedeni teto metody muzeme kontrolovat pocet odeslanych zprav vasi implementaci.
     */
    public List<Pair<String, Message>> act(Queue<Message> inbox, String disseminationProcess) {
        // Od DisseminationProcess muzete dostat zpravu typu DeadProcessMessage, ktera Vas
        // informuje o spravne detekovanem ukoncenem procesu.
        // DisseminationProcess muzete poslat zpravu o detekovanem "mrtvem" procesu.
        // Zprava musi byt typu PFDMessage.

        List<Pair<String, Message>> messagesToSend = new ArrayList<>();
        pokeProcesses(messagesToSend);

        respondToMessages(inbox, messagesToSend);

//      Timeouts
//        1. increment all awaiting acknowledgement timers
        awaitingAck = awaitingAck.entrySet().stream()
                .collect(Collectors.toMap(Entry::getKey, entry -> entry.getValue() + 1));

//        2. check for timeouts
        List<String> timedOutProcesses = awaitingAck.entrySet().stream()
                .filter(entry -> entry.getValue() > maxDelayForMessages)
                .map(Entry::getKey)
                .toList();

//        3. for each timed out process,
//          send further poke message,
//          through other random process that is not in the list of timed out processes
        timedOutProcesses.forEach(process -> {
            String randomProcess = otherProcesses.stream()
                    .filter(p -> !timedOutProcesses.contains(p))
                    .findAny()
                    .orElse(null);
            if (randomProcess != null && messagesSent < upperBoundOnMessages) {
                messagesToSend.add(new Pair<>(randomProcess, SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.FURTHER_POKE, process, process)));
                messagesSent++;

//            move process from awaitingAck to awaitingFurtherAck
                awaitingFurtherAck.put(process, 0);
                awaitingAck.remove(process);
            } else {
                System.err.println("No process to send further poke message to");
            }
        });

//        4. increment all awaiting further acknowledgement timers
        awaitingFurtherAck = awaitingFurtherAck.entrySet().stream()
                .collect(Collectors.toMap(Entry::getKey, entry -> entry.getValue() + 1));

//        5. check for further timeouts
        List<String> timedOutFurtherProcesses = awaitingFurtherAck.entrySet().stream()
                .filter(entry -> entry.getValue() > maxDelayForMessages * 2)
                .map(Entry::getKey)
                .toList();

//        6. for each timed out further process,
//          check with Dissemination if truly failed
        timedOutFurtherProcesses.forEach(process -> {
            messagesToSend.add(new Pair<>(disseminationProcess, new PFDMessage(process)));
            awaitingFurtherAck.remove(process);
        });


        return messagesToSend;
    }

    private void pokeProcesses(List<Pair<String, Message>> messagesToSend){
//        1. select random amount of processes to poke
        int amountOfProcessesToPoke = random.nextInt(upperBoundOnMessages / 2);

//        2. select random processes to poke
        List<String> processesToPoke = IntStream.range(0, amountOfProcessesToPoke)
                .mapToObj(i -> otherProcesses.get(random.nextInt(otherProcesses.size())))
                .toList();

//        3. generate poke message for each process
        processesToPoke.forEach(process -> {
            messagesToSend.add(new Pair<>(process, SWIMMessage.createShortDistanceMessage(SWIMMessage.MessageType.POKE)));
            messagesSent++;
            awaitingAck.put(process, 0);
        });

    }

    private void respondToMessages(Queue<Message> inbox, List<Pair<String, Message>> messagesToSend){
        while (!inbox.isEmpty()) {
            Message message = inbox.poll();
            if (message instanceof SWIMMessage msg) {

                switch (msg.getType()){
                    case ACK ->
                            awaitingAck.remove(msg.sender);
                    case FURTHER_ACK ->
                            awaitingAck.remove(msg.getWaitingForAckProcess());
                    case REPOST_ACK ->
                            messagesToSend.add(new Pair<>(msg.getWaitingForAckProcess(), SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.FURTHER_ACK, msg.getWaitingForAckProcess(), msg.getNotRespondingProcess())));
                    case FURTHER_POKE ->
                            messagesToSend.add(new Pair<>(msg.sender, SWIMMessage.createLongDistanceMessage(SWIMMessage.MessageType.REPOST_ACK, msg.getWaitingForAckProcess(), msg.getNotRespondingProcess())));
                    case POKE ->
                            messagesToSend.add(new Pair<>(msg.sender, SWIMMessage.createShortDistanceMessage(SWIMMessage.MessageType.ACK)));
                }
            } else if (message instanceof DeadProcessMessage deadProcessMessage) {
                System.out.println("Spravne detekovan ukonceny proces " + deadProcessMessage.getProcessID());
            }
        }
    }

}
