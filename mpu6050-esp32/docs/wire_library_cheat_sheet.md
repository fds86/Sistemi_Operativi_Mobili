# Wire Cheat Sheet (ESP32 + PlatformIO)

Questa guida raccoglie le 10 funzioni piu utili della libreria Wire per comunicazione I2C su ESP32.

## Setup

### 1) begin(sda, scl, frequency)
Inizializza il bus I2C con pin e frequenza.

Parametri:
- sda: GPIO usato come linea dati I2C (es. 21 su molte board ESP32).
- scl: GPIO usato come linea clock I2C (es. 22 su molte board ESP32).
- frequency: frequenza bus in Hz. Valori tipici: 100000 (standard), 400000 (fast mode).

```cpp
Wire.begin(21, 22, 400000);
```

### 2) setClock(frequency)
Cambia la frequenza I2C dopo l'inizializzazione.

Parametri:
- frequency: nuova frequenza in Hz per il bus gia avviato.
- nota pratica: tutti i dispositivi collegati devono supportare la frequenza scelta.

```cpp
Wire.setClock(400000);
```

### 3) setTimeOut(ms)
Imposta il timeout delle transazioni I2C.

Parametri:
- ms: timeout in millisecondi per operazioni bloccanti I2C.
- effetto: evita blocchi infiniti se uno slave non risponde.

```cpp
Wire.setTimeOut(50);
```

### 4) getTimeOut()
Legge il timeout attualmente impostato.

Parametri:
- nessuno.

Ritorno:
- uint16_t: timeout corrente in millisecondi.

```cpp
uint16_t t = Wire.getTimeOut();
```

## Scrittura verso uno slave

### 5) beginTransmission(address)
Apre una trasmissione verso un device I2C.

Parametri:
- address: indirizzo I2C slave (7 bit), spesso scritto in esadecimale (es. 0x68).
- nota: questa chiamata prepara il buffer TX, non invia ancora sul bus.

```cpp
Wire.beginTransmission(0x68);
```

### 6) write(value)
Scrive un byte o un buffer nel pacchetto in uscita.

Parametri:
- value (byte singolo): un valore 8 bit da inviare.
- buffer + length (overload): puntatore ai dati e numero di byte da inviare.
- nota: i dati vengono effettivamente trasmessi con endTransmission().

```cpp
Wire.write(0x6B);
Wire.write((uint8_t)0x00);
```

### 7) endTransmission(sendStop)
Chiude la trasmissione e invia i dati.

- sendStop = true: invia STOP
- sendStop = false: repeated start

Parametri:
- sendStop: controlla la condizione finale del bus.
- uso tipico: false quando devi fare subito requestFrom sullo stesso slave.

Ritorno (uint8_t):
- 0: successo
- 1..4: errori di protocollo/bus (dipendono dal core)

```cpp
uint8_t err = Wire.endTransmission(false);
```

## Lettura da uno slave

### 8) requestFrom(address, len, stopBit)
Richiede len byte dal device.

Parametri:
- address: indirizzo I2C slave (7 bit).
- len: numero di byte da leggere.
- stopBit: true invia STOP a fine lettura, false mantiene il bus per sequenze combinate.

Ritorno:
- numero di byte realmente messi nel buffer RX.

```cpp
Wire.requestFrom((uint8_t)0x68, (size_t)14, (bool)true);
```

### 9) available()
Ritorna quanti byte sono disponibili nel buffer RX.

Parametri:
- nessuno.

Ritorno:
- int: numero di byte leggibili subito con read().

```cpp
if (Wire.available() >= 14) {
  // dati pronti
}
```

### 10) read()
Legge un byte dal buffer RX.

Parametri:
- nessuno.

Ritorno:
- int: byte letto (0..255), o -1 se non ci sono dati.

```cpp
uint8_t hi = Wire.read();
uint8_t lo = Wire.read();
int16_t v = (int16_t)((hi << 8) | lo);
```

## Flusso tipico per leggere registri MPU-6050

```cpp
Wire.beginTransmission(0x68);
Wire.write(0x3B);          // registro iniziale
Wire.endTransmission(false);

Wire.requestFrom((uint8_t)0x68, (size_t)14, (bool)true);
while (Wire.available() < 14) {}
```

## Note rapide

- Wire su ESP32 e gia inclusa nel framework Arduino usato da PlatformIO.
- Non serve copiare manualmente Wire nel progetto.
- Se hai warning su requestFrom, usa tipi espliciti come nell'esempio sopra.

## Valori esadecimali in Wire.write (MPU-6050)

Quando usi Wire, i valori in formato 0xNN sono byte in esadecimale.

- 0x indica base 16 (hex).
- ogni coppia di cifre hex rappresenta 1 byte (8 bit).
- range di un byte: 0x00..0xFF (0..255 in decimale).

Esempi tipici del tuo sensore:

- 0x68: indirizzo I2C del dispositivo MPU-6050 (AD0 a GND).
- 0x69: indirizzo alternativo (AD0 a VCC).
- 0x6B: registro PWR_MGMT_1.
- 0x3B: primo registro dati accelerometro (ACCEL_XOUT_H).

Sequenza concettuale di scrittura su un registro:

```cpp
Wire.beginTransmission(0x68); // seleziona lo slave
Wire.write(0x6B);             // seleziona il registro
Wire.write(0x00);             // valore da scrivere
Wire.endTransmission(true);   // invia i dati sul bus
```

In questo esempio:

- 0x6B non e il dato finale: e il "puntatore" al registro interno.
- 0x00 e il dato scritto in quel registro.

Conversioni rapide utili:

- 0x68 = 104
- 0x6B = 107
- 0x3B = 59

Perche usare hex:

- i datasheet riportano registri e bitmask in esadecimale,
- e piu facile allineare codice e documentazione,
- e piu immediato leggere pattern a livello di byte/bit.
