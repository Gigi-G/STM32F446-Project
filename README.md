# Laboratorio di Sistemi a Microcontrollore



## Ascensore

Si consideri un ascensore di cui si desidera gestire l'automazione. L'ascensore gestisce 3 piani i quali sono simulati dai tasti *X* (primo piano), *Y* (secondo piano) e *Z* (terzo piano). La pressione di uno dei tasti provoca la partenza dell'ascensore verso il piano selezionato, qualora l'ascensore non sia in moto, oppure la **prenotazione** del piano selezionato qualora l'ascensore sia già in corsa.

Nel caso della prenotazione, se il piano selezionato sarà una destinazione futura, questo deve essere segnalato tramite un log e il piano non deve essere considerato.

Nel caso si selezioni il piano in cui l'ascensore si trova già, non bisogna fare nulla e segnalarlo attraverso un log.

La corsa dell'ascensore deve essere gestita nel seguente modo:

- Si chiudono le porte; tale evento dura 1.5s ed è indicato dal lampeggio del led rosso con periodo di 200ms.
- Si avvia la corsa dell'ascensore verso il piano; durante la corsa occorre visualizzare, sul display, il piano via via raggiunto, simulando la presenza dell'ascensore tra due piani; in particolare, quando l'ascensore si trova tra il piano P e il piano P+1, occorre visualizzare sul display il numero P seguito da un trattino.
- Durante la corsa occorre far lampeggiare il led giallo con periodo di 200ms.
- La velocità dell'ascensore deve essere regolabile.
- All'arrivo al piano, si aprono le porte; tale evento dura 1.5s ed è indicato dal lampeggio del led verde con periodo di 200ms.

Alla pressione del tasto T il sistema deve entrare in uno stato di configurazione dal quale deve essere possibile, tramite uno dei trimmer, regolare la velocità dell'ascensore in un intervallo compreso tra 1 s/piano e 0.4 s/piano, con granularità di 0.1 s/piano (visualizzare sul display il valore via via selezionato). Premendo nuovamente il tasto T si rientra nella normale operatività.

Utilizzare la UART per stampare messaggi di log che segnalano gli eventi che accadono durante l'operatività.

### Esempio di log

```bash
-----------------------------------------------------------------------------------------
System is running...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The new destination is floor 3
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Closing the door at floor 1...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The floor is   1-...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The floor is    2...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The floor is   2-...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Opening the door at floor 3...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
System enter setup mode...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The selected speed is 0.6
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
System quit setup mode...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
You already are at floor 3
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The new destination is floor 2
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Closing the door at floor 3...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The new reservation is for floor 1
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The floor is   2-...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Opening the door at floor 2...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The new destination is floor 1
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Closing the door at floor 2...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
The floor is   1-...
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
Opening the door at floor 1...
-----------------------------------------------------------------------------------------
```

