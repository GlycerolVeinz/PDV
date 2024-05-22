# "Návod" pro používání frameworku DSand
(from https://cw.fel.cvut.cz/wiki/courses/b4b36pdv/dsand)

Pro simulaci problémů, se kterými se můžete setkat při implementaci distribuovaných systémů, budeme používat v rámci předmětu framework DSand. Tento framework nevykonává řešení skutečně na více počítačích, nicméně nám umožňuje simulovat některé ze standartních problémů, které mohou nastat, konkrétně například:

 - Konfigurovatelnou (respektive variabilní) dobu potřebnou pro doručování zpráv (tj., latenci sítě)
 - Nespolehlivé doručování sítě (tj., nestabilitu sítě)
 - Různé rychlosti procesů
 - Havárie (nedostupnost) procesů

Vaším úkolem bude implementovat logiku procesů (potomků třídy DSProcess), návod k frameworku je proto koncipován z pohledu procesů.

## Architektura simulace

Simulace distribuovaného systému běží na jediném počítači a jediném vláknu. Jednotlivé procesy jsou proto probouzeny simulací (je zavolána metoda `act()` procesu) a dostávají tak čas na vykonání potřebné práce (především zpracování a odeslání zpráv). Vzhledem k tomu, že procesy neběží současně, musí proto metoda `act()` vždy skončit v konečném (krátkém) čase!

Procesy jsou ve frameworku reprezentovány instancemi třídy odděděné od třídy `cz.cvut.fel.agents.pdv.dsand.DSProcess`. Každý proces je identifikován v rámci simulace podle svého identifikátoru (`String`), který slouží i jako cílová adresa při zasílání zpráv. Identifikátor aktuálního procesu můžete zjisti z atributu `id` procesu. Často při implementaci pořebujete znát i identifikátory ostatních procesů, se kterými budete komunikovat - ty vám typicky předáváme v konstruktoru potomka třídy `DSProcess`, který máte za úkol implementovat.

Pro zasílání zpráv používejte metodu `send(String rcpt, Message message)`. `rcpt` je identifikátor procesu, kterému chcete zprávu zaslat, `message` je instance zprávy, kterou chcete zaslat. Zprávy si můžete naimplementovat vlastního typu, avšak musíte dodržet, že zprávy dědí od třídy `cz.cvut.fel.agents.pdv.dsand.Message` a musí obsahovat pouze atributy, které jsou serializovatelné. Pokud je aktuálnímu procesu doručena zpráva, je umístěna do jeho fronty `inbox` (`java.util.Queue`). Doporučujeme vám v každé iteraci (při každém vykonání metody `act(…)`) zpracovat všechny zprávy v `inbox` – v opačném případě může dojít k zahlcení systému nepřečtenými zprávami.

V případě, že chcete simulaci ukončit (a tím ukončit běh aplikace), můžete zavolat metodu terminateAll().

## Nastavení a průběh simulace

Průběh simulace je parametrizovaný instancí třídy  `cz.cvut.fel.agents.pdv.dsand.DSConfig` (respektive jejího potomka).

 - Metoda `getDeliveryDelay(String src, String dst)` ovlivňuje počet simulačních kroků potřebných pro doručení zprávy, kterou proces s identifikátorem src zasílá procesu s identifikátorem `dst`.
 - Metoda `getReliability(String src, String dst)` ovlivňuje spolehlivost komunikačního kanálu od procesu `src` k procesu `dst`. Pokud proces s identifikátorem `src` zasílá zprávu procesu s identifikátorem `dst`, tato zpráva bude ztracena s pravděpodobností 1 - `getReliability(src, dst)`.
 - Metoda `getProcessesToWake()` vrací procesy (respektive, jejich identifikátory), které se mají v aktuálním cyklu simulace probudit (tj., má se zavolat jejich metoda `act()`). To umožňuje například simulovat různé rychlosti běhu procesů (procesy se v návratové hodnotě metody `getProcessesToWake()` objevují s různou frekvencí), tak jejich havárii (proces se v návratové hodnotě metody `getProcessesToWake()` po určité době přestane objevovat).
